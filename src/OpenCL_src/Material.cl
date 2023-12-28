
typedef struct {
    unsigned int height, width;
    unsigned char * data;
} Texture;

typedef struct {

    Texture texture;

    float useTexture;
    float isEmissive;
    float isTransparent;

    Vec3 ambiant_color;
    Vec3 diffuse_color;
    Vec3 specular_color;

    float emissiveIntensity;
    float shininess;
    float IOR;
    float transparency;

    float metallic, roughness, ao;

} Material;


const int MESH_MAX_NUMBER = 1000;
Material materials[MESH_MAX_NUMBER];
