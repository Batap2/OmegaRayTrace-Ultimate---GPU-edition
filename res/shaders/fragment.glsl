#version 330 core

const int LIGHTS_MAX_SIZE = 64;
const int lightStructSize = 7;

// Inputs passed in from the vertex shader
in vec3 mynormal;
in vec4 myvertex;
in vec2 myuv;

in vec3 m_diffuse;
in vec4 m_mra;
in vec3 useTexture;

// Uniform variable modelview
uniform mat4 modelview;
uniform vec3 camPos;

struct Light
{
        vec3 pos;
        vec3 color;
        float intensity;
};

uniform int lights_number;
uniform float lights[LIGHTS_MAX_SIZE * lightStructSize];

uniform int render_mode;

uniform sampler2D diffuse_texture;
uniform sampler2D float_texture;

// Output of the fragment shader
out vec4 fragColor;

float PI = 3.141592;
// ---------------------------------- PBR FUNCTIONS ------------------------------

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// --------------------------------------------------------------------------------

vec3 phong(vec3 lightPos, vec3 viewPos, vec3 normal, vec3 lightColor, vec3 objectColor, vec4 fragPos, float ambientStrength, float specularStrength, float shininess)
{
    vec3 lightDir = normalize(lightPos - vec3(fragPos));
    vec3 viewDir = normalize(viewPos - vec3(fragPos));
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 ambient = ambientStrength * objectColor;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * objectColor;

    vec3 specular = vec3(0.0);
    if (diff > 0.0) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
        specular = specularStrength * spec * lightColor;
    }

    vec3 result = (ambient + diffuse + specular) * lightColor;

    return result;
}


void main (void){

    vec3 finalColor = vec3(0,0,0);

    if(render_mode == 1)
    {
        vec3 colorFromTexture = texture(float_texture, myuv).rgb;
        finalColor = colorFromTexture;
    } else
    {

        vec3 viewDir = normalize(camPos - vec3(myvertex));
        vec3 N = mynormal;

        vec3 albedo;

        if(useTexture.r != 0){
            albedo = texture(diffuse_texture, myuv).rgb;
        } else {
            albedo = m_diffuse;
        }

        float metallic = m_mra[0];
        float roughness = m_mra[1];
        float ao = m_mra[3];

        vec3 resultPBR = vec3(0.0);

        // Calcul de F0, le coefficient de reflexion d'une surface parfaitement orthogonale a une lumière
        // encore une fois, valeur arbitraire pour simplifier. 0.04 est une valeur correct pour du metal.
        vec3 F0 = vec3(0.04);
        F0      = mix(F0, albedo, metallic);

        for(int i = 0; i < lights_number; i++){

            int offset = i*lightStructSize;

            vec3 lightPos = vec3(lights[offset+0], lights[offset+1], lights[offset+2]);
            vec3 lightColor = vec3(lights[offset+3], lights[offset+4], lights[offset+5]);
            float lightIntensity = lights[offset+6];

            vec3 lightDir = normalize(lightPos - vec3(myvertex));
            vec3 Halfway = normalize(viewDir + lightDir);

            // Calcul de la radiance
            float distanceFromL = length(lightDir);
            float attenuation = 1/(distanceFromL*distanceFromL)*lightIntensity;
            vec3 radiance = lightColor * attenuation;

            // Modele de reflectance de Cook-Torrance
            float NDF = DistributionGGX(N, Halfway, roughness);
            float G   = GeometrySmith(N, viewDir, lightDir, roughness);
            vec3 F    = fresnelSchlick(max(dot(Halfway, viewDir), 0.0), F0);

            // Composante spéculaire, KS: valeur de Fresnel de la composante spéculaire, KD: l'énergie non reflétée par le spéculaire
            vec3 kS = F;
            vec3 kD = vec3(1.0) - kS;
            kD *= 1.0 - metallic;

            // calcul du BRDF
            vec3 numerator    = NDF * G * F;
            float denominator = 4.0 * max(dot(N, viewDir), 0.0) * max(dot(N, lightDir), 0.0) + 0.0001;
            vec3 specularPBR     = numerator / denominator;

            // somme des calculs de PBR, Lambert Diffuse
            float NdotL = max(dot(N, lightDir), 0.0);
            resultPBR += (kD * albedo / PI + specularPBR) * radiance * NdotL;
        }

        vec3 ambientPBR = vec3(0.03) * albedo * ao;
        vec3 colorPBR = ambientPBR + resultPBR;

        vec3 pbrColor = resultPBR + ambientPBR;

        finalColor = pbrColor;
    }


    fragColor = vec4(finalColor, 1.0f);
    //fragColor = vec4(1,1,1,1);
}