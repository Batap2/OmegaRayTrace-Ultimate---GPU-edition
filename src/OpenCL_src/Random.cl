unsigned int *randomSeed;


float randomFloat(float min, float max) {

    unsigned long multiplier = 48271L;
    unsigned long modulus = 2147483647L;

	*randomSeed= (multiplier * (*randomSeed)) % modulus + 564651 << 01;
    double scaledRandom = *randomSeed / ((double)modulus + 1);
    return (float)(scaledRandom * (max - min) + min);
}