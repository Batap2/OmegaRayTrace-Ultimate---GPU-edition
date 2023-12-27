#version 330 core

// Vertex array layout inputs to the vertex shader
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 in_m_diffuse;
layout (location = 4) in vec4 in_m_mra;
layout (location = 5) in vec3 in_useTexture;


// Uniform variables
uniform mat4 modelview;
uniform mat4 projection;
uniform int render_mode;

// Additional outputs to pass stuff to fragment shader
out vec3 mynormal;
out vec4 myvertex;
out vec2 myuv;

out vec3 m_ambiant;
out vec3 m_diffuse;
out vec3 m_specular;
out vec4 m_mra;
out vec3 useTexture;

void main(){

	if(render_mode == 1){
		gl_Position = vec4(position, 1.0f);
	} else
	{
		gl_Position = projection * modelview * vec4(position, 1.0f);
	}

	mynormal = normal;
	myvertex = vec4(position, 1.0f);
	myuv = texCoord;

	m_diffuse = in_m_diffuse;
	m_mra = in_m_mra;
	useTexture = in_useTexture;
}