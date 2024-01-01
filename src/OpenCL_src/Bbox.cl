typedef struct {
    Vec3 bbmin;
    Vec3 bbmax;
    int triangle_nbr;
    int offset_triangle;
} Bbox;

bool intersectBbox(Ray ray, Bbox bbox) {
    // Calcul des intersections avec la boîte englobante
    float t1 = (bbox.bbmin.x - ray.origin.x) / ray.direction.x;
    float t2 = (bbox.bbmax.x - ray.origin.x) / ray.direction.x;
    float t3 = (bbox.bbmin.y - ray.origin.y) / ray.direction.y;
    float t4 = (bbox.bbmax.y - ray.origin.y) / ray.direction.y;
    float t5 = (bbox.bbmin.z - ray.origin.z) / ray.direction.z;
    float t6 = (bbox.bbmax.z - ray.origin.z) / ray.direction.z;

    float t_min = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
    float t_max = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

    return !(t_max < 0 || t_min > t_max);
}