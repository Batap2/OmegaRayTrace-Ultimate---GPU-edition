typedef struct {
    Vec3 normal;
    Vec3 center;
    Material mat;
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