
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

Vec3 addFloat(Vec3 a, float b){
    a.x = a.x + b;
    a.y = a.y + b;
    a.z = a.z + b;
    return a;
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

Vec3 divide(Vec3 a, float b){
	a.x = a.x/b;
	a.y = a.y/b;
	a.z = a.z/b;
	return a;
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

Vec3 lerp(Vec3 a, Vec3 b, float value) {
    Vec3 interpolated_vec;
    interpolated_vec.x = a.x + (b.x - a.x) * value;
    interpolated_vec.y = a.y + (b.y - a.y) * value;
    interpolated_vec.z = a.z + (b.z - a.z) * value;
    return interpolated_vec;
}

//-----------------------------------------RANDOM---------------------------------------------------------//


Vec3 randomizeInHemiSphere(Vec3 normal, float roughness) {
    float r1 = randomFloat(0.0f, 1.0f);
    float r2 = randomFloat(0.0f, 1.0f);

    float phi = 2 * M_PI * r1; // Angle around the hemisphere
    float cos_theta = pow(1 - r2, 1.0f / (roughness + 1));
    float sin_theta = sqrt(1 - cos_theta * cos_theta);

    // Construct a coordinate system using the normal vector
    Vec3 tangent = (Vec3){0.0f,0.0f,0.0f};
	Vec3 bitangent = (Vec3){0.0f,0.0f,0.0f};

    if (fabs(normal.x) > 0.1f)
        tangent = (Vec3){normal.y, -normal.x, 0.0f};
    else
        tangent = (Vec3){0.0f, normal.z, -normal.y};

    tangent = (Vec3){normal.y, -normal.x, 0.0f};
    bitangent = (Vec3){normal.z * tangent.x, normal.z * tangent.y, -normal.x * tangent.x - normal.y * tangent.y};

    // Convert spherical coordinates to Cartesian coordinates
    Vec3 randomVec;
    randomVec.x = sin_theta * cos(phi) * tangent.x + sin_theta * sin(phi) * bitangent.x + cos_theta * normal.x;
    randomVec.y = sin_theta * cos(phi) * tangent.y + sin_theta * sin(phi) * bitangent.y + cos_theta * normal.y;
    randomVec.z = sin_theta * cos(phi) * tangent.z + sin_theta * sin(phi) * bitangent.z + cos_theta * normal.z;

    return randomVec;
}

Vec3 randomizeInHemiSphere_fast(Vec3 normal, float roughness)
{
	
    Vec3 randomVec = (Vec3){randomFloat(-roughness,roughness), randomFloat(-roughness,roughness), randomFloat(-roughness,roughness)};

	randomVec = add(randomVec, normal);
	return normalize( add(randomVec, scale(normal, 0.1f) ));
}

unsigned int GPURandomInt;