#version 150

uniform mat4 projection_matrix;

in vec4 vPosition;
in vec4 normal;
in vec2 vTexCoord;
out vec2 texCoord;
void main() 
{	
	texCoord = vTexCoord;
	gl_Position = projection_matrix*vPosition;
} 
