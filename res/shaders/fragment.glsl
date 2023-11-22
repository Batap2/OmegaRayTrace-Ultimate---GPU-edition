# version 330 core

const int LIGHTS_MAX_SIZE = 64;

// Max number of light sources
const int num_lights = 5;

// Inputs passed in from the vertex shader
in vec3 mynormal;
in vec4 myvertex;

// Uniform variable modelview
uniform mat4 modelview;
uniform vec3 camPos;

layout(std140) uniform LightsPosBuffer {
    vec3 data[LIGHTS_MAX_SIZE];
} lightsPosBuffer;

layout(std140) uniform LightsColorBuffer {
vec3 data[LIGHTS_MAX_SIZE];
} lightsColorBuffer;

// Uniform variables to do with lighting
uniform vec4 light_posn[num_lights];
uniform vec4 light_col[num_lights];

// Uniform variable for object properties
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shininess;

// Output of the fragment shader
out vec4 fragColor;

void main (void){

    vec3 colorFromTexture = vec3(0.8,0.2,0.2);

    vec3 lightColor = vec3(1,1,1);

    vec3 lp = vec3(2,2,2);
    vec3 vp = camPos;
    vec3 N = mynormal;

    vec3 lightDir = normalize(lp - vec3(myvertex));
    vec3 viewDir = normalize(vp - vec3(myvertex));
    vec3 reflectDir = reflect(-lightDir, N);

    vec3 ambient = 0.1 * colorFromTexture;
    vec3 diffuse = max(dot(N, lightDir), 0.0) * colorFromTexture;
    vec3 specular = lightColor * pow(max(dot(viewDir, reflectDir), 0), 50) * 0.2;
    vec4 classicPhongColor = vec4(ambient + diffuse + specular, 1.0);

    fragColor = classicPhongColor;
}