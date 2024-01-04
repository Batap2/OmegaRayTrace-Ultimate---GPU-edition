enum ObjectType
{
	SPHERE,
	PLANE,
	TRIANGLE,
};

typedef struct
{
    bool intersectionExists;
    float t;
    Vec3 normal;
    Vec3 position;
    Vec3 position2;
	Material material;
	enum ObjectType objectType;
	float2 uv_hit;
	float2 uv0,uv1,uv2;

} HitData;