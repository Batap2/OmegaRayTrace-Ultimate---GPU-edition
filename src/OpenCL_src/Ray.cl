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