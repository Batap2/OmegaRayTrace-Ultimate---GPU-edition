typedef struct {
	Vec3 color;
	Vec3 pos;

	float intensity;
} Light;

const int LIGHT_MAX_NUMBER = 32;
Light lights[LIGHT_MAX_NUMBER];
int lightNumber = 0;

void addLight(Vec3 pos, Vec3 color, float intensity) {

	if(lightNumber == LIGHT_MAX_NUMBER){
		return;
	}

    lights[lightNumber].color = color;
    lights[lightNumber].pos = pos;
    lights[lightNumber].intensity = intensity;

	lightNumber++;
}

void removeLight(int index) {
    if (index < 0 || index >= lightNumber) {
        return;
    }

    for (int i = index; i < lightNumber - 1; ++i) {
        lights[i] = lights[i + 1];
    }

    lightNumber--;
}