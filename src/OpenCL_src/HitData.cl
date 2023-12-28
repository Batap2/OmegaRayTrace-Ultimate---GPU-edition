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
	Material material;
	enum ObjectType objectType;
} HitData;