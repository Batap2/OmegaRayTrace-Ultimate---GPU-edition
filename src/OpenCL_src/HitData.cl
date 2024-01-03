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
    Vec3 normal;           // ajout normal
    Vec3 position;         // ajout position
    Vec3 bounce_direction; // Ajout bounce direction
    Vec3 ColorTexture;
	Material material;
	enum ObjectType objectType;
	float2 uv_hit;
	float2 uv0,uv1,uv2;

} HitData;