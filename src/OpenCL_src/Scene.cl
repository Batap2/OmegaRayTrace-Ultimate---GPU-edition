typedef struct {
    Sphere spheres[100];
    int numSpheres;

    Plane planes[100];
    int numPlanes;

    Triangle triangles[200];
    int numTriangles;
} Scene;

Scene *mainScene;
Scene mainScene2;
Vec3 skyColor;

void addSphere(Vec3 center, float radius, int mat)
{
Sphere s = (Sphere){center,radius,mat};
mainScene2.spheres[mainScene2.numSpheres] = s;
mainScene2.numSpheres++;
}


HitData shootRay(Vec3 origin, Vec3 direction)
{
	Ray ray = createRay(origin, direction);

	float t;
	float t_min = 0.000001f;
	float t_max = FLT_MAX;
	HitData HD;
	HD.intersectionExists = false;

	for (int s = 0; s < mainScene2.numSpheres; s++) {
			if (intersectSphere(ray, mainScene2.spheres[s], &t, t_min, &t_max, &HD)) {}
			}
	for (int p = 0; p < mainScene2.numPlanes; p++) {
		if (intersectPlane(ray, mainScene2.planes[0], &t, t_min, &t_max, &HD)) {}
		}
	for (int tIndex = 0; tIndex < mainScene2.numTriangles; tIndex++) {
		if (intersectTriangle(ray, mainScene2.triangles[tIndex], &t, t_min, &t_max, &HD)) {}
	}

	return HD;
}


// --------------------------------------------------- PHONGI -------------------------------------------------------

Vec3 computePhong(HitData *HD, Ray *ray, Vec3 camPos){

	Vec3 result = (Vec3){0.0f,0.0f,0.0f};

	for(int light_i = 0; light_i < lightNumber; ++light_i)
	{
		Light light = lights[light_i];

		float shininess = 400;
		float specularStrength = 1;

		Vec3 lightDir = normalize(subtract(light.pos, HD->position));
		Vec3 viewDir = normalize(subtract(camPos, HD->position));
		Vec3 reflectDir = reflect(negate(lightDir), HD->normal);

		Vec3 ambient = scale(HD->material.diffuse_color, 0.0f);
		float diff = max(dot(HD->normal, lightDir), 0.0f);
		Vec3 diffuse = scale(scale(HD->material.diffuse_color, diff), light.intensity);

		Vec3 specular = (Vec3){0.0f,0.0f,0.0f};
		if (diff > 0.0) {
			Vec3 halfwayDir = normalize(add(lightDir,viewDir));
			float spec = pow(max(dot(HD->normal, halfwayDir), 0.0f), shininess);
			specular = scale(scale(light.color, specularStrength * spec), light.intensity);
		}

		Vec3 newColor = multiply(light.color ,add(add(diffuse, specular), ambient));
		result = add(result, newColor);
	}

    return clampMin(result,1.0f);
	
}


// ------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------- PBR -------------------------------------------------------
Vec3 fresnelSchlick(float cosTheta, Vec3 F0)
{

	Vec3 result = scale( add(addFloat(F0, 1.0f), negate(F0)), pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f));

	return result;
}

float DistributionGGX(Vec3 N, Vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = 3.14159265359f * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float num   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeometrySmith(Vec3 N, Vec3 V, Vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

Vec3 computePBR(HitData *HD, Ray *ray, Vec3 camPos)
{
    Vec3 result = (Vec3){0.0f,0.0f,0.0f};

	Vec3 viewDir = normalize(subtract(camPos, HD->position));

	Vec3 F0 = (Vec3){0.04f,0.04f,0.04f};
	F0 = lerp(F0, HD->material.diffuse_color, HD->material.roughness);
	
	for(int light_i = 0; light_i < lightNumber; ++light_i)
	{
		Light light = lights[light_i];
		Vec3 lightDir = normalize(subtract(light.pos, HD->position));
		Vec3 Halfway = normalize(add(viewDir, lightDir));
		

		// Calcul de la radiance
		float distanceFromL = length(lightDir);
		float attenuation = 1/distanceFromL*distanceFromL*light.intensity;
		Vec3 radiance = scale(light.color, attenuation);

		
		// Modele de reflectance de Cook-Torrance
		float NDF = DistributionGGX(HD->normal, Halfway, HD->material.roughness);
		float G = GeometrySmith(HD->normal, viewDir, lightDir, HD->material.roughness);
		Vec3 F = fresnelSchlick(max(dot(Halfway, viewDir), 0.0f), F0);

		
		// Composante spéculaire, KS: valeur de Fresnel de la composante spéculaire, KD: l'énergie non reflétée par le spéculaire
		Vec3 kS = F;
		Vec3 kD = subtract((Vec3){1.0f,1.0f,1.0f}, kS);
		kD = scale(kD, 1.0f - HD->material.metallic);


		// calcul du BRDF
		Vec3 numerator = scale(F, NDF * G);
		float denominator = 4.0f * max(dot(HD->normal, viewDir), 0.0f) * max(dot(HD->normal, lightDir), 0.0f) + 0.0001f;
		Vec3 specularPBR = divide(numerator, denominator);


		// somme des calculs de PBR, Lambert Diffuse
		float NdotL = max(dot(HD->normal, lightDir), 0.0f);
		
		Vec3 resultPBR = multiply(add(divide(multiply(kD,HD->material.diffuse_color),3.14159265359f),specularPBR),scale(radiance,NdotL));
		result = add(result, resultPBR);
	}

	Vec3 ambient = scale(HD->material.diffuse_color, 0.03f * 1.0f);

	result = add(result, ambient);

    return clampMin(result,1.0f);
}


// ---------------------------------------------------------------------------------------------------------------

Vec3 computeColor(Ray *ray, Vec3 camPos, int nbBounce)
{


	Vec3 finalColor = (Vec3){0.0f,0.0f,0.0f};
	Vec3 contribution = (Vec3){1.0f,1.0f,1.0f};
	Vec3 previousColor = (Vec3){0.0f,0.0f,0.0f};

	Ray ray2 = createRay(ray->origin, ray->direction);

	HitData HD;

	for(int bounce = 0; bounce < nbBounce; bounce++)
	{

		if(bounce != 0){
			previousColor = HD.material.diffuse_color;
		}

		HD = shootRay(ray2.origin, ray2.direction);

		HD.normal = randomizeInHemiSphere_fast(HD.normal, HD.material.roughness);


		if(!HD.intersectionExists)
		{

			finalColor = add(finalColor, multiply(contribution, skyColor));
			break;
		} else {
			
			contribution = multiply(contribution, HD.material.diffuse_color);
			
			Vec3 emissiveColor = scale(HD.material.diffuse_color, HD.material.emissiveIntensity);
			


			Vec3 mixedColor = lerp(scale(previousColor, HD.material.emissiveIntensity), emissiveColor, 0.5f);

			finalColor = add(finalColor, mixedColor);
		}

		ray2.origin = HD.position;
		ray2.direction = reflect(ray2.direction, HD.normal);

	}


	if(nbBounce == 1){
		return multiply(finalColor, contribution);
	}
	
	return finalColor;
}