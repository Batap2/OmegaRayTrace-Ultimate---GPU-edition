typedef struct {
    Vec3 vertex1;
    Vec3 vertex2;
    Vec3 vertex3;
    int mat;
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
        HD->material = materials[triangle.mat];
		HD->objectType = TRIANGLE;
        return true;
    }

    return false;
}