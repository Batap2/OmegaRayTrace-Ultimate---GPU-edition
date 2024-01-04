typedef struct {
    Vec3 vertex1;
    Vec3 vertex2;
    Vec3 vertex3;
    float2 uv0, uv1, uv2;
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
        // Edge 0
        Vec3 edge0 = subtract(triangle.vertex2, triangle.vertex1);
        Vec3 edge1 = subtract(triangle.vertex3, triangle.vertex2);
        Vec3 edge2 = subtract(triangle.vertex1, triangle.vertex3);

        Vec3 vp0 = subtract(hit_position, triangle.vertex1);
        Vec3 vp1 = subtract(hit_position, triangle.vertex2);
        Vec3 vp2 = subtract(hit_position, triangle.vertex3);

        Vec3 tn = cross(subtract(triangle.vertex2, triangle.vertex1),subtract(triangle.vertex3,triangle.vertex1));

        float alpha = dot(tn, cross(edge0, vp0)); // Le cross product est oriente selon si l'angle est aigu ou obtu
        float beta = dot(tn, cross(edge1, vp1)); // On verifie si le cross product est oriente comme la normale de notre triangle
        float gamma = dot(tn, cross(edge2, vp2)); // Si le dot product est negatif c'est que les cross products sont inverses (n:-1 et cp:1 ou n:1 et cp:-1)
            if (alpha < 0 || beta < 0 || gamma < 0){
                return false;
            }
                   beta = beta / dot(tn, tn);
                   gamma = gamma / dot(tn, tn);
                   alpha = 1 - beta - gamma;
        *t = t_hit;
        *t_max = t_hit;
        HD->intersectionExists = true;
        HD->position = add(ray.origin, scale(ray.direction, *t));
		HD->normal = N;
        HD->material = materials[triangle.mat];
		HD->objectType = TRIANGLE;
		// Interpoler les coordonnées UV de l'intersection
         // Interpoler les coordonnées UV de l'intersection
         HD->uv_hit.x = beta * triangle.uv0.x + gamma * triangle.uv1.x + alpha * triangle.uv2.x;
         HD->uv_hit.y = beta * triangle.uv0.y + gamma * triangle.uv1.y + alpha * triangle.uv2.y;

        //HD->uv_hit.x = u;
        //HD->uv_hit.y = v;
        HD->uv0 = triangle.uv0;
        HD->uv1 = triangle.uv1;
        HD->uv2 = triangle.uv2;
        return true;
    }

    return false;
}