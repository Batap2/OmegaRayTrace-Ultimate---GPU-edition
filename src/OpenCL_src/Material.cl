
typedef struct {
    unsigned int height, width;
    unsigned char * data;
    int offset_tex;
} Texture;

const int TEXTURE_MAX_NUMBER = 300000000;
unsigned char textures[TEXTURE_MAX_NUMBER];

Vec3 getPixelColor(Texture tex, float u, float v) {
    int width = (int)textures[tex.offset_tex];
    int height = (int)textures[tex.offset_tex+1];
   int x = (int)(u * width);
    int y = (int)(v * height);

    x = clamp(x, 0, (int)width - 1);
    y = clamp(y, 0, (int)height - 1);

   int pixelIndex =tex.offset_tex +2 + (y * width + x) * 3;

    float red = (float)textures[pixelIndex] / 255.0f;
    float green = (float)textures[pixelIndex + 1] / 255.0f;
    float blue = (float)textures[pixelIndex + 2] / 255.0f;
    Vec3 result = ((Vec3){red, green, blue});
    return result;
    //return (Vec3){0.,0.,0.};
}

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