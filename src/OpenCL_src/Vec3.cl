

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

    return reflectedVec;
}

Vec3 clampMin(Vec3 a, float clamp){
	a.x = min(a.x, clamp);
	a.y = min(a.y, clamp);
	a.z = min(a.z, clamp);
	return a;
}
