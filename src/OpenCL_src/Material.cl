
typedef struct {
    unsigned int height, width;
    unsigned char * data;
    unsigned int offset;
} Texture;

const int TEXTURE_MAX_NUMBER = 300000000;
unsigned char textures[TEXTURE_MAX_NUMBER];

typedef struct {

    Texture texture;

    bool useTexture;
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
int material_nbr;

int createMaterial(Vec3 ambiantcolor,Vec3 diffusecolor,Vec3 specularcolor, float shininess, float metallic, float roughness, float ao)
{
    materials[material_nbr].ambiant_color = ambiantcolor;
    materials[material_nbr].diffuse_color = diffusecolor;
    materials[material_nbr].specular_color = specularcolor;
    materials[material_nbr].shininess = shininess;
    materials[material_nbr].metallic = metallic;
    materials[material_nbr].roughness = roughness;
    materials[material_nbr].ao = ao;
    material_nbr++;

    return material_nbr - 1 ;
}