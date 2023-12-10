#version 330 core

// Vertex array layout inputs to the vertex shader
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

// Uniform variables
uniform mat4 modelview;
uniform mat4 projection;

// Additional outputs to pass stuff to fragment shader
out vec3 mynormal;
out vec4 myvertex;
out vec2 myuv;

void main(){
    gl_Position = projection * modelview * vec4(position, 1.0f);
	mynormal = normal;
	myvertex = vec4(position, 1.0f);
	myuv = texCoord;
}