
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

        if(*t < *t_max)
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
    Sphere spheres[100];
    int numSpheres;

    Plane planes[100];
    int numPlanes;
} Scene;




__kernel void render(__global float* fb, int max_x, int max_y) {
    int i = get_global_id(0);
    int j = get_global_id(1);

    if ((i < max_x) && (j < max_y)) {
        int pixel_index = j * max_x * 3 + i * 3;

        // Créez une instance de la structure Ray
        Ray ray;

        // Utilisez normalize pour normaliser le vecteur
        ray.direction.x = 2.0 * ((float)i + 0.5) / (float)max_x - 1.0;
        ray.direction.y = 1.0 - 2.0 * ((float)j + 0.5) / (float)max_y;
        ray.direction.z = 0.2;  // Vous pouvez ajuster cette valeur selon vos besoins

        // Définissez l'origine du rayon (vous pouvez laisser l'origine à (0, 0, 0) ou définir une origine différente si nécessaire)
        ray.origin = (Vec3){0.0, 0.0, 0.0};


        Scene scene;
                    scene.numSpheres = 1;
                    scene.spheres[0].center = (Vec3){0.0, 0.0, -5.0};
                    scene.spheres[0].radius = 3.0;

                    scene.numPlanes = 1;
                    scene.planes[0].center = (Vec3){0.0, 5.0, 0.0};
                    scene.planes[0].normal = (Vec3){0.0, 1.0, 0.0};

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
                    if(HD.intersectionExists == false)
                    {
                       fb[pixel_index + 0] = 0.0;
                       fb[pixel_index + 1] = 0.0;
                       fb[pixel_index + 2] = 0.0;
                    }
    }
}
