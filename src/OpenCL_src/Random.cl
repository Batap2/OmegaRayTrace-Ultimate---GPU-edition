unsigned int *randomSeed;

float randomFloat(float min, float max) {

    unsigned long multiplier = 48271L;
    unsigned long modulus = 2147483647L;
	
	*randomSeed= (multiplier * (*randomSeed)) % modulus + 564651;
    double scaledRandom = *randomSeed / ((double)modulus + 1);
    return (float)(scaledRandom * (max - min) + min);
}


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
	Vec3 randomVec = (Vec3){randomFloat(0.0f,roughness), randomFloat(0.0f,roughness), randomFloat(0.0f,roughness)};

	randomVec = add(randomVec, normal);
	return normalize( add(randomVec, scale(normal, 0.1f) ));
}

unsigned int GPURandomInt;