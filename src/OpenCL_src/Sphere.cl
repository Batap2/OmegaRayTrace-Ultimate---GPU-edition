// Structure représentant une sphère
typedef struct {
    Vec3 center;
    float radius;
    int mat;
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
			HD->material = materials[sphere.mat];
			HD->objectType = SPHERE;
			return true;
        }
    }

    // Pas d'intersection
    return false;
}