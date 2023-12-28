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
	Material material;
	enum ObjectType objectType;
} HitData;