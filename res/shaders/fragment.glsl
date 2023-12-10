#version 330 core

const int LIGHTS_MAX_SIZE = 64;
const int lightStructByteSize = 7;

// Max number of light sources
const int num_lights = 5;

// Inputs passed in from the vertex shader
in vec3 mynormal;
in vec4 myvertex;
in vec2 myuv;

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
uniform float lights[LIGHTS_MAX_SIZE * lightStructByteSize];

uniform sampler2D diffuse_texture;

// Output of the fragment shader
out vec4 fragColor;

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


    for(int i = 0; i < lights_number; i++)
    {
        int offset = i*lightStructByteSize;

        vec3 colorFromTexture = texture(diffuse_texture, myuv).rgb;


        vec3 lp = vec3(lights[offset+0], lights[offset+1], lights[offset+2]);
        vec3 lightColor = vec3(lights[offset+3], lights[offset+4], lights[offset+5]);

        vec3 newCol = phong(lp, camPos, mynormal, lightColor, colorFromTexture, myvertex, 0, 0.5, 32);

        finalColor += newCol;

        finalColor = min(finalColor, vec3(1,1,1));
        //finalColor = texture(diffuse_texture, myuv).rgb;
    }

    fragColor = vec4(finalColor, 1.0f);
    //fragColor = vec4(1,1,1,1);
}