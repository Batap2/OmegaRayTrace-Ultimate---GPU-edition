typedef struct {
    Sphere spheres[100];
    int numSpheres;

    Plane planes[100];
    int numPlanes;

    Triangle triangles[300000];
    int numTriangles;

    Bbox bboxes[100];
    int numBboxes;

} Scene;

Scene *mainScene;
Scene mainScene2;
Vec3 skyColor;
int scene_size;

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

        Bbox b = mainScene2.bboxes[0];
        if(intersectBbox(ray,b))
        {
            unsigned int tri_nbr = b.triangle_nbr;
            for (int tIndex = 0; tIndex < tri_nbr; tIndex++) {
              if (intersectTriangle(ray, b.trianglesOfBox[tIndex], &t, t_min, &t_max, &HD)) {}
            }
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

	bool hasHittedTransparentObject = false;

	bool rayInObject = false;


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
		} else
		{
            Vec3 diffuse;
			// absorb light of the light beam with the materials
			if(HD.material.useTexture)
			{

			
			int texX = HD.uv_hit.x * HD.material.texture.width;
			int texY = HD.uv_hit.y * HD.material.texture.height;


            int texIndex = (int)(texY * HD.material.texture.width + texX) *3;
            unsigned off = HD.material.texture.offset;
            float r = textures[off+texIndex];
            float g = textures[off+texIndex+1];
            float b = textures[off+texIndex+2];
			diffuse = (Vec3){r/255,g/255,b/255};
			}
            else
            {
            diffuse = HD.material.diffuse_color;
            }
			contribution = multiply(contribution, diffuse);





			// add light to light beam uniquely if the material is emissive
			Vec3 emissiveColor = scale(diffuse, HD.material.emissiveIntensity);

			Vec3 mixedColor_emissive = lerp(scale(previousColor, HD.material.emissiveIntensity), emissiveColor, 0.5f);

			finalColor = add(finalColor, mixedColor_emissive);


		}


		if(HD.material.isTransparent != 0.0f)
		{


			if(rayInObject)
			{
				// TODO : mettre HD.material.IOR à la place de 1.4f (sur l'autre ligne refract aussi)
				ray2.direction = refract(ray2.direction, negate(HD.normal), 1.4f, 1.0f);
				if(HD.objectType == SPHERE)
				{
					ray2.origin = add(HD.position2, scale(ray2.direction, 0.01f));
				}

			} else
			{


				// ---------- reflexion of transparent materials

				// this part is incredibly dirty, the correct way is to make a recursive function but opencl doesn't support it.
				// this part needs to be externalised in an other function but sadly my time is up.

				Ray reflectRayFromTransparent = createRay(HD.position, reflect(ray2.direction, HD.normal));

				HitData H_reflectFromTransparent = shootRay(reflectRayFromTransparent.origin, reflectRayFromTransparent.direction);


				if(!HD.intersectionExists)
				{

					finalColor = add(finalColor, multiply(contribution, skyColor));
					break;
				} else
				{
                Vec3 diffuse2;
                // absorb light of the light beam with the materials
                if(H_reflectFromTransparent.material.useTexture)
                {
                						int texX = HD.uv_hit.x * HD.material.texture.width;
                            			int texY = HD.uv_hit.y * HD.material.texture.height;


                                        int texIndex = (int)(texY * HD.material.texture.width + texX) *3;
                                        unsigned off = HD.material.texture.offset;
                                        float r = textures[off+texIndex];
                                        float g = textures[off+texIndex+1];
                                        float b = textures[off+texIndex+2];
                            			diffuse2 = (Vec3){r/255,g/255,b/255};
                }
                else
                {
                diffuse2 = H_reflectFromTransparent.material.diffuse_color;
                }
					contribution = multiply(contribution, lerp(diffuse2, (Vec3){1.0f,1.0f,1.0f}, 0.8f));


					// add light to light beam uniquely if the material is emissive
					Vec3 emissiveColor = scale(H_reflectFromTransparent.material.diffuse_color, H_reflectFromTransparent.material.emissiveIntensity);
					Vec3 mixedColor_emissive = lerp(scale(previousColor, H_reflectFromTransparent.material.emissiveIntensity), emissiveColor, 0.5f);
					finalColor = add(finalColor, mixedColor_emissive);
				}

				// ---------- reflexion of transparent materials


				ray2.origin = add(HD.position, scale(ray2.direction, 0.01f));
				ray2.direction = refract(ray2.direction, HD.normal, 1.0f, 1.4f);
			}

			rayInObject = !rayInObject;
			hasHittedTransparentObject = true;

		} else
		{

			ray2.origin = HD.position;
			ray2.direction = reflect(ray2.direction, HD.normal);
		}


	}

	
	return finalColor;
}