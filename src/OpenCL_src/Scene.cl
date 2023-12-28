typedef struct {
    Sphere spheres[100];
    int numSpheres;

    Plane planes[100];
    int numPlanes;

    Triangle triangles[200];
    int numTriangles;
} Scene;

Scene *mainScene;
Vec3 skyColor;

HitData shootRay(Vec3 origin, Vec3 direction)
{
	Ray ray = createRay(origin, direction);

	float t;
	float t_min = 0.01;
	float t_max = FLT_MAX;
	HitData HD;
	HD.intersectionExists = false;

	for (int s = 0; s < mainScene->numSpheres; s++) {
			if (intersectSphere(ray, mainScene->spheres[s], &t, t_min, &t_max, &HD)) {}
			}
	for (int p = 0; p < mainScene->numPlanes; p++) {
		if (intersectPlane(ray, mainScene->planes[0], &t, t_min, &t_max, &HD)) {}
		}
	for (int tIndex = 0; tIndex < mainScene->numTriangles; tIndex++) {
		if (intersectTriangle(ray, mainScene->triangles[tIndex], &t, t_min, &t_max, &HD)) {}
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

Vec3 computePBR(HitData *HD, Ray *ray)
{
    // Vec3 returnColor = (Vec3){mat.diffuse_color.x,mat.diffuse_color.y,mat.diffuse_color.z};


    // return returnColor;
}


// ---------------------------------------------------------------------------------------------------------------

Vec3 computeColor(Ray *ray, Vec3 camPos, int nbBounce)
{

	HitData HD = shootRay(ray->origin, ray->direction);
	Vec3 finalColor = computePhong(&HD, ray, camPos);

	for(int bounce = 0; bounce < nbBounce; bounce++)
	{
		if(!HD.intersectionExists)
		{
			return (Vec3){0.0f,0.3f,0.0f};
		}

		Ray reflectedRay;
		reflectedRay.direction = reflect(ray->direction, HD.normal);
		reflectedRay.origin = HD.position;

		HD = shootRay(reflectedRay.origin, reflectedRay.direction);
		
		Vec3 reflectedColor = computePhong(&HD, ray, camPos);
		reflectedColor = (Vec3){0.0f,1.0f,0.0f};

		add(finalColor, reflectedColor);
	}

	if(!HD.intersectionExists){
		return skyColor;
	}

	
	return finalColor;
}