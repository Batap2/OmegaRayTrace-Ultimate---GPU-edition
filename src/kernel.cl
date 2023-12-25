
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

typedef struct
{
    bool intersectionExists;
    float t;
    Vec3 normal;           // ajout normal
    Vec3 position;         // ajout position
    Vec3 bounce_direction; // Ajout bounce direction
    Vec3 color;
} HitData;


// Structure représentant une sphère
typedef struct {
    Vec3 center;
    float radius;
} Sphere;

// Fonction pour tester l'intersection entre un rayon et une sphère
bool intersectSphere(Ray ray, Sphere sphere, float* t, float t_min, float* t_max, HitData* HD) {
    // Vecteur depuis l'origine du rayon jusqu'au centre de la sphère
    Vec3 oc = subtract(ray.origin, sphere.center);

    // Coefficients de l'équation quadratique
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - sphere.radius * sphere.radius;

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
          HD->color = (Vec3){1.0,0.0,0.0};
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
            HD->color = (Vec3){0.0,1.0,0.0};
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

    double D = dot(triangle.vertex1, N);

    // 1) Check that the ray is not parallel to the triangle:
    const float epsilon = 1e-6f;
    if (length(cross(N, d)) < epsilon) {
        return false;
    }

    // 2) Check that the triangle is "in front of" the ray:
    double t_hit = (D - dot(o, N)) / dot(d, N);
    hit_position = add(o, scale(d, t_hit));
    double orientation = dot(d, N);
    if (t_hit > 0 && t_hit >= t_min && t_hit < *t_max) {
        // 3) Check that the intersection point is inside the triangle:
        Vec3 C;

        // Edge 0
        Vec3 edge0 = subtract(triangle.vertex2, triangle.vertex1);
        Vec3 vp0 = subtract(hit_position, triangle.vertex1);
        C = cross(edge0, vp0);
        if (dot(N, C) < 0 || dot(N, C) > denom) {
            return false;
        }

        // Edge 1
        Vec3 edge1 = subtract(triangle.vertex3, triangle.vertex2);
        Vec3 vp1 = subtract(hit_position, triangle.vertex2);
        C = cross(edge1, vp1);
        u = dot(N, C);
        if (u < 0 || u > denom) {
            return false;
        }

        // Edge 2
        Vec3 edge2 = subtract(triangle.vertex1, triangle.vertex3);
        Vec3 vp2 = subtract(hit_position, triangle.vertex3);
        C = cross(edge2, vp2);
        v = dot(N, C);
        if (v < 0 || v > denom) {
            return false;
        }

        // We have an intersection
        u /= denom;
        v /= denom;

        *t = t_hit;
        *t_max = t_hit;
        HD->intersectionExists = true;
        HD->color = (Vec3){0.0, 0.0, 1.0};  // Set color for triangle intersection
        return true;
    }

    return false;
}

typedef struct {
    Sphere spheres[100];
    int numSpheres;

    Plane planes[100];
    int numPlanes;

    Triangle triangles[100];
    int numTriangles;
} Scene;



//Fonction principale du kernel -> Rendu par raytracing 'une image de la scène
__kernel void render(__global float* fb, int max_x, int max_y,  __global float* cameraData,__global float* vertices,__global unsigned int* indices, int numTri) {
    int i = get_global_id(0);
    int j = get_global_id(1);

   Vec3 cameraPos = (Vec3){cameraData[0], cameraData[1], cameraData[2]};
   Vec3 cameraDirection = (Vec3){cameraData[3+0], cameraData[3+1], cameraData[3+2]};
   Vec3 cameraRight = (Vec3){cameraData[6+0], cameraData[6+1], cameraData[6+2]};
   Vec3 cameraUp = cross(cameraDirection, cameraRight);
   float FOV = 60.;

    float aspectRatio = (float)max_x / (float)max_y;
    float tanFOV = tan(0.5 * radians(FOV));

    if ((i < max_x) && (j < max_y)) {
        int pixel_index = j * max_x * 3 + i * 3;

        //u et v (entre 0 et 1) relatif au screenspace
        Ray ray;
        float u = (2.0 * ((float)i + 0.5) / (float)max_x - 1.0) * aspectRatio * tanFOV;
        float v = (1.0 - 2.0 * ((float)j + 0.5) / (float)max_y) * tanFOV;

        //Setup des coordonnées du rayon courant
        ray.direction = normalize(add(scale(cameraRight, u), add(scale(cameraUp, v), cameraDirection)));
        ray.origin = cameraPos;


        Scene scene;
                    scene.numSpheres = 1;
                    scene.spheres[0].center = (Vec3){0.0, 0.0, -7.0};
                    scene.spheres[0].radius = 1.0;

                    scene.numPlanes = 0;
                    scene.planes[0].center = (Vec3){0.0, 5.0, 0.0};
                    scene.planes[0].normal = (Vec3){0.0, 1.0, 0.0};

                    scene.numTriangles = 1;
                    scene.triangles[0].vertex1 = (Vec3){0.0,0.0,-5.0};
                    scene.triangles[0].vertex2 = (Vec3){-1.0,1.0,-5.0};
                    scene.triangles[0].vertex3 = (Vec3){0.0,1.0,-5.0};

            // Testez l'intersection entre le rayon et la sphère
            float t;
            float t_min = 0.01;
            float t_max = FLT_MAX;
            HitData HD;
            HD.intersectionExists = false;
            for (int s = 0; s < scene.numSpheres; s++) {
                    if (intersectSphere(ray, scene.spheres[s], &t, t_min, &t_max, &HD)) {
                        // S'il y a une intersection, la couleur dépend de la distance
                        // Copiez les valeurs de couleur dans le tableau fb
                        fb[pixel_index + 0] = HD.color.x;
                        fb[pixel_index + 1] = HD.color.y;
                        fb[pixel_index + 2] = HD.color.z;
                        }
                    }
            for (int p = 0; p < scene.numPlanes; p++) {
                   if (intersectPlane(ray, scene.planes[0], &t, t_min, &t_max, &HD)) {
                       // S'il y a une intersection, la couleur dépend de la distance
                         fb[pixel_index + 0] = HD.color.x;
                         fb[pixel_index + 1] = HD.color.y;
                         fb[pixel_index + 2] = HD.color.z;
                         }
                   }
            for (int tIndex = 0; tIndex < scene.numTriangles; tIndex++) {
                            if (intersectTriangle(ray, scene.triangles[tIndex], &t, t_min, &t_max, &HD)) {
                                // S'il y a une intersection, la couleur dépend de la distance
                                fb[pixel_index + 0] = HD.color.x;
                                fb[pixel_index + 1] = HD.color.y;
                                fb[pixel_index + 2] = HD.color.z;
                            }
                    }
                    if(HD.intersectionExists == false)
                    {
                       fb[pixel_index + 0] = 0.0;
                       fb[pixel_index + 1] = 0.0;
                       fb[pixel_index + 2] = 0.0;
                    }
    }
}
