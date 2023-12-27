


typedef struct {
    float x;
    float y;
    float z;
} Vec3;

float length(Vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 normalize(Vec3 v) {
    float len = length(v);
    if (len > 0.0) {
        v.x /= len;
        v.y /= len;
        v.z /= len;
    }
    return v;
}

Vec3 add(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vec3 scale(Vec3 v, float s) {
    Vec3 result;
    result.x = v.x * s;
    result.y = v.y * s;
    result.z = v.z * s;
    return result;
}

Vec3 subtract(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

// Fonction pour calculer le produit scalaire de deux vecteurs Vec3
float dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

Vec3 multiply(Vec3 a, Vec3 b)
{
	a.x = a.x * b.x;
	a.y = a.y * b.y;
	a.z = a.z * b.z;
	return a;
}

Vec3 divide(Vec3 a, float b){
	a.x = a.x/b;
	a.y = a.y/b;
	a.z = a.z/b;
	return a;
}

Vec3 negate(Vec3 vec){
	vec.x = -vec.x;
	vec.y = -vec.y;
	vec.z = -vec.z;
	return vec;
}

Vec3 reflect(Vec3 vec, Vec3 normal)
{
    float dotProduct = vec.x * normal.x + vec.y * normal.y + vec.z * normal.z;

    Vec3 reflectedVec;
    reflectedVec.x = vec.x - 2 * dotProduct * normal.x;
    reflectedVec.y = vec.y - 2 * dotProduct * normal.y;
    reflectedVec.z = vec.z - 2 * dotProduct * normal.z;

    return normalize(reflectedVec);
}

Vec3 clampMin(Vec3 a, float clamp){
	a.x = min(a.x, clamp);
	a.y = min(a.y, clamp);
	a.z = min(a.z, clamp);
	return a;
}

Vec3 randomizeVec(Vec3 a, float maxAngle){
	
}

typedef struct {
    unsigned int height, width;
    unsigned char * data;
} Texture;

typedef struct {

    Texture texture;

    float useTexture;
    float isEmissive;
    float isTransparent;

    Vec3 ambiant_color;
    Vec3 diffuse_color;
    Vec3 specular_color;

    float emissiveIntensity;
    float shininess;
    float IOR;
    float transparency;

    float metallic, roughness, ao;

} Material;

Material testMat;
Material testMat2;


typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

Ray createRay(Vec3 origin, Vec3 direction) {
    Ray ray;
    ray.origin = origin;
    ray.direction = direction;
    return ray;
}

Vec3 pointAtParameter(Ray ray, float t) {
    // Équation O + t*D
    return add(ray.origin, scale(ray.direction, t));
}

enum ObjectType
{
	SPHERE,
	PLANE,
	TRIANGLE,
};

typedef struct
{
    bool intersectionExists;
    float t;
    Vec3 normal;           // ajout normal
    Vec3 position;         // ajout position
    Vec3 bounce_direction; // Ajout bounce direction
	Material material;
	enum ObjectType objectType;
} HitData;


// Structure représentant une sphère
typedef struct {
    Vec3 center;
    float radius;
} Sphere;

// Fonction pour tester l'intersection entre un rayon et une sphère
bool intersectSphere(Ray ray, Sphere sphere, float* t, float t_min, float* t_max, HitData* HD) {

    Vec3 co = subtract(ray.origin, sphere.center);

    // Coefficients de l'équation quadratique
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(co, ray.direction);
    float c = dot(co, co) - sphere.radius * sphere.radius;

    // Calcul du discriminant
    float discriminant = b * b - 4.0 * a * c;

    // Si le discriminant est positif, il y a une intersection
    if (discriminant > 0.0) {
        // Calcule les deux solutions de l'équation quadratique
        float root1 = (-b - sqrt(discriminant)) / (2.0 * a);
        float root2 = (-b + sqrt(discriminant)) / (2.0 * a);

        // Retourne la plus petite valeur positive (plus proche de l'origine du rayon)
        *t = (root1 < root2) ? root1 : root2;

        if(*t < *t_max && *t > t_min)
        {
			*t_max = *t;
			HD->intersectionExists = true;
			HD->position = add(ray.origin, scale(ray.direction, *t));
			HD->normal = normalize(subtract(HD->position, sphere.center));
			HD->material = testMat2;
			HD->objectType = SPHERE;
			return true;
        }
    }

    // Pas d'intersection
    return false;
}


typedef struct {
    Vec3 normal;
    Vec3 center;
} Plane;

bool intersectPlane(Ray ray, Plane plane, float* t, float t_min, float* t_max, HitData* HD) {
    float denominator = dot(ray.direction, plane.normal);

    // Éviter la division par zéro
    if (fabs(denominator) > 1e-6) {
        *t = dot(subtract(plane.center, ray.origin), plane.normal) / denominator;

        // Assurez-vous que t est dans la plage spécifiée
        if (*t >= t_min && *t < *t_max) {
            *t_max = *t;
            HD->intersectionExists = true;
            HD->material = testMat;
			HD->objectType = PLANE;
            return true;  // Il y a une intersection avec le plan
        }
    }

    return false;  // Pas d'intersection avec le plan
}

typedef struct {
    Vec3 vertex1;
    Vec3 vertex2;
    Vec3 vertex3;
} Triangle;


bool intersectTriangle(Ray ray, Triangle triangle, float* t, float t_min, float* t_max, HitData* HD) {
    Vec3 d = ray.direction;
    Vec3 o = ray.origin;

    float u, v;
    Vec3 hit_position;

    Vec3 s0s1 = subtract(triangle.vertex2, triangle.vertex1);
    Vec3 s0s2 = subtract(triangle.vertex3, triangle.vertex1);
    Vec3 N = normalize(cross(s0s1, s0s2));
    float denom = dot(N, N);
    float area2 = sqrt(denom);

    double D = dot(triangle.vertex1, N);
    float dotN_Raydir = dot(N, d);

    // 1) Check that the ray is not parallel to the triangle:
    const float epsilon = 1e-6f;
    if (fabs(dotN_Raydir) < epsilon) {
        return false;
    }

    // 2) Check that the triangle is "in front of" the ray:
    float t_hit = (D - dot(o, N)) / dotN_Raydir;
    if (t_hit > 0 && t_hit >= t_min && t_hit < *t_max) {
        // Calculate intersection point
        hit_position = add(o, scale(d, t_hit));

        // 3) Check that the intersection point is inside the triangle:
        Vec3 C;

        // Edge 0
        Vec3 edge0 = subtract(triangle.vertex2, triangle.vertex1);
        Vec3 vp0 = subtract(hit_position, triangle.vertex1);
        C = cross(edge0, vp0);
        u = dot(N, C);
        if (u < 0) {
            return false;
        }

        // Edge 1
        Vec3 edge1 = subtract(triangle.vertex3, triangle.vertex2);
        Vec3 vp1 = subtract(hit_position, triangle.vertex2);
        C = cross(edge1, vp1);
        v = dot(N, C);
        if (v < 0) {
            return false;
        }

        // Edge 2
        Vec3 edge2 = subtract(triangle.vertex1, triangle.vertex3);
        Vec3 vp2 = subtract(hit_position, triangle.vertex3);
        C = cross(edge2, vp2);
        u = dot(N, C);
        if (u < 0) {
            return false;
        }

        // We have an intersection
        u /= area2;
        v /= area2;

        *t = t_hit;
        *t_max = t_hit;
        HD->intersectionExists = true;
        HD->position = add(ray.origin, scale(ray.direction, *t));
		HD->normal = N;
        HD->material = testMat;
		HD->objectType = TRIANGLE;
        return true;
    }

    return false;
}

typedef struct {
	Vec3 color;
	Vec3 pos;

	float intensity;
} Light;

const int LIGHT_MAX_NUMBER = 32;
Light lights[LIGHT_MAX_NUMBER];
int lightNumber = 0;

void addLight(Vec3 pos, Vec3 color, float intensity) {

	if(lightNumber == LIGHT_MAX_NUMBER){
		return;
	}

    lights[lightNumber].color = color;
    lights[lightNumber].pos = pos;
    lights[lightNumber].intensity = intensity;

	lightNumber++;
}

void removeLight(int index) {
    if (index < 0 || index >= lightNumber) {
        return;
    }

    for (int i = index; i < lightNumber - 1; ++i) {
        lights[i] = lights[i + 1];
    }

    lightNumber--;
}


typedef struct {
    Sphere spheres[100];
    int numSpheres;

    Plane planes[100];
    int numPlanes;

    Triangle triangles[200];
    int numTriangles;
} Scene;

Scene *mainScene;


HitData shootRay(Vec3 origin, Vec3 direction)
{
	Ray ray = createRay(origin, direction);

	float t;
	float t_min = 0.000001f;
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
	//Vec3 finalColor = computePhong(&HD, ray, camPos);
	Vec3 finalColor = (Vec3){0.0f,0.0f,0.0f};

	if(!HD.intersectionExists){
		return (Vec3){0.0f,1.0f,0.0f};
	}

	for(int bounce = 0; bounce < nbBounce; bounce++)
	{

		Ray reflectedRay;
		reflectedRay.direction = reflect(ray->direction, HD.normal);
		reflectedRay.origin = HD.position;
				
		HD = shootRay(reflectedRay.origin, reflectedRay.direction);

		Vec3 reflectedColor;

		if(!HD.intersectionExists)
		{
			reflectedColor = (Vec3){1.0f,1.0f,1.0f};
		} else {
			reflectedColor = computePhong(&HD, ray, camPos);
		}


		

		finalColor = add(finalColor, reflectedColor);
	}


	
	return divide(finalColor, nbBounce);
}




//Fonction principale du kernel -> Rendu par raytracing 'une image de la scène
__kernel void render(__global float* fb, int max_x, int max_y,  __global float* cameraData,__global float* vertices,__global unsigned int* indices, int numMesh,__global unsigned int* split_meshes,__global unsigned int* split_meshes_tri)
{
	int i = get_global_id(0);
	int j = get_global_id(1);

	// TODO : importe la skycolor
	Vec3 skyColor = (Vec3){0.0f,0.0f,0.0f};

	Vec3 cameraPos = (Vec3){cameraData[0], cameraData[1], cameraData[2]};
	Vec3 cameraDirection = (Vec3){cameraData[3+0], cameraData[3+1], cameraData[3+2]};
	Vec3 cameraRight = (Vec3){cameraData[6+0], cameraData[6+1], cameraData[6+2]};
	Vec3 cameraUp = cross(cameraDirection, cameraRight);
	float FOV = 60.;

    float aspectRatio = (float)max_x / (float)max_y;
    float tanFOV = tan(0.5 * radians(FOV));

    if ((i < max_x) && (j < max_y)) {
        int pixel_index = j * max_x * 3 + i * 3;


		// Calcul des coordonnées du rayon
		Ray ray;
		float u = (2.0 * ((float)i + 0.5) / (float)max_x - 1.0) * aspectRatio *tanFOV;
		float v = (1.0 - 2.0 * ((float)j + 0.5) / (float)max_y)* tanFOV;

		ray.direction = normalize(add(scale(cameraRight, u), add(scale(cameraUp, v), cameraDirection)));
		ray.origin = cameraPos;

		Scene scene;

		scene.numSpheres = 1;
		int x = 0;
		scene.spheres[0].center = (Vec3){0.6f,-0.7f,1.0f};
		scene.spheres[0].radius = 0.3f;


		scene.numPlanes = 0;
		scene.planes[0].center = (Vec3){0.0, 5.0, 0.0};
		scene.planes[0].normal = (Vec3){0.0, 1.0, 0.0};


		scene.numTriangles = 0;
		unsigned int offset_vertex = 0;
		unsigned int offset_index = 0;
		int mesh_num = numMesh;

		for(int mesh_id = 0; mesh_id < mesh_num; mesh_id++)
		{
			unsigned int vertex_nbr = split_meshes[mesh_id];
			unsigned int tri_nbr = split_meshes_tri[mesh_id];
			unsigned int index_nbr = tri_nbr *3;

			for(int tri = 0; tri < tri_nbr; tri++)
			{
				int current_id = offset_index + tri*3 ;
				int id0 = current_id;
				int id1 = current_id + 1;
				int id2 = current_id + 2;

				Vec3 s0 = (Vec3){vertices[(offset_vertex+indices[id0])*3+0],vertices[(offset_vertex+indices[id0])*3+1],vertices[(offset_vertex+indices[id0])*3+2]};
				Vec3 s1 = (Vec3){vertices[(offset_vertex+indices[id1])*3+0],vertices[(offset_vertex+indices[id1])*3+1],vertices[(offset_vertex+indices[id1])*3+2]};
				Vec3 s2 = (Vec3){vertices[(offset_vertex+indices[id2])*3+0],vertices[(offset_vertex+indices[id2])*3+1],vertices[(offset_vertex+indices[id2])*3+2]};

				scene.triangles[tri + offset_index/3].vertex1 = s0;
				scene.triangles[tri + offset_index/3].vertex2 = s1;
				scene.triangles[tri + offset_index/3].vertex3 = s2;
				scene.numTriangles++;
			}
			offset_index += index_nbr;
			offset_vertex += vertex_nbr;
		}

		mainScene = &scene;


		testMat.diffuse_color = (Vec3){1.0f,0.2f,0.2f};
		testMat.metallic = 0.0f;
		testMat.roughness = 0.5f;

		testMat2.diffuse_color = (Vec3){0.5f,1.0f,0.2f};
		testMat2.metallic = 0.0f;
		testMat2.roughness = 0.5f;

		addLight((Vec3){-0.75f,1.0f,1.2f}, (Vec3){1.0f,0.8f,0.55f}, 0.1f);
		//addLight((Vec3){0.75f,1.0f,1.2f}, (Vec3){0.85f,0.95f,1.0f}, 0.1f);


		
		int bounce = 1;
	
		Vec3 out_color = computeColor(&ray, cameraPos, bounce);

		fb[pixel_index + 0] = out_color.x;
		fb[pixel_index + 1] = out_color.y;
		fb[pixel_index + 2] = out_color.z;

		
    }
}
