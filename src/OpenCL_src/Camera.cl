

typedef struct {

	Vec3 cameraPos;
	Vec3 cameraDirection;
	Vec3 cameraRight;
	Vec3 cameraUp;
	float FOV;
    float aspectRatio;
    float tanFOV;
} Camera;

Ray getRayfromCamera(Camera camera, int i, int j, int max_x, int max_y)
{
		float u = (2.0 * ((float)i + 0.5) / (float)max_x - 1.0) * camera.aspectRatio *camera.tanFOV;
		float v = (1.0 - 2.0 * ((float)j + 0.5) / (float)max_y)* camera.tanFOV;
        Ray ray;
		ray.direction = normalize(add(scale(camera.cameraRight, u), add(scale(camera.cameraUp, v), camera.cameraDirection)));
		ray.origin = camera.cameraPos;
		return ray;
}