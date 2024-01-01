typedef struct {
    Vec3 bbmin;
    Vec3 bbmax;
    int triangle_nbr;
    int offset_triangle;
    Triangle* trianglesOfBox;
    struct Bbox* left;
    struct Bbox* right;
} Bbox;


void swap(Triangle* a, Triangle* b) {
    Triangle temp = *a;
    *a = *b;
    *b = temp;
}

Vec3 computeTriangleCentroid(Triangle triangle) {
    Vec3 centroid;
    centroid.x = (triangle.vertex1.x + triangle.vertex2.x + triangle.vertex3.x) / 3.0f;
    centroid.y = (triangle.vertex1.y + triangle.vertex2.y + triangle.vertex3.y) / 3.0f;
    centroid.z = (triangle.vertex1.z + triangle.vertex2.z + triangle.vertex3.z) / 3.0f;
    return centroid;
}

bool compareTriangles(Triangle a, Triangle b, int axis) {
    Vec3 centroidA = computeTriangleCentroid(a);
    Vec3 centroidB = computeTriangleCentroid(b);

    switch (axis) {
        case 0:
            return centroidA.x > centroidB.x;
        case 1:
            return centroidA.y > centroidB.y;
        case 2:
            return centroidA.z > centroidB.z;
        default:
            return false;
    }
}

void bubbleSort(Triangle* triangles, int size, int axis) {
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (compareTriangles(triangles[j], triangles[j + 1], axis)) {
               swap(&triangles[j], &triangles[j + 1]);
            }
        }
    }
}

void KdTreeFromTriangles(Bbox* kd, Triangle* triangles_)
{
    Vec3 min = (Vec3){INFINITY, INFINITY, INFINITY};
    Vec3 max = (Vec3){-INFINITY, -INFINITY, -INFINITY};

    for (unsigned int i = 0; i < kd->triangle_nbr; ++i)
    {
        Triangle T = triangles_[i];
        Vec3 c = computeTriangleCentroid(T);

        min.x = fmin(min.x, c.x);
        min.y = fmin(min.y, c.y);
        min.z = fmin(min.z, c.z);

        max.x = fmax(max.x, c.x);
        max.y = fmax(max.y, c.y);
        max.z = fmax(max.z, c.z);
    }

    kd->bbmin = min;
    kd->bbmax = max;
}

void generateKdTree(__global Bbox* b, unsigned int depth, int axis)
{
    if (b->triangle_nbr < 2 || depth < 1)
    {
        return;
    }

    bubbleSort(b->trianglesOfBox, b->triangle_nbr, axis);

    unsigned int middle = b->triangle_nbr / 2;

    Bbox b_l;
    b->left = &b_l;
    b_l.triangle_nbr = middle;
    b_l.trianglesOfBox = b->trianglesOfBox;
    KdTreeFromTriangles(&b_l, b_l.trianglesOfBox);

    Bbox b_r;
    b->right = &b_r;
    b_r.triangle_nbr = b->triangle_nbr - middle;
    b_r.trianglesOfBox = b->trianglesOfBox + middle;
    KdTreeFromTriangles(&b_r, b_r.trianglesOfBox);

    // Appels récursifs pour générer les sous-arbres
    //generateKdTree(b->left, depth - 1, (axis + 1) % 3);
    //generateKdTree(b->right, depth - 1, (axis + 1) % 3);
}

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