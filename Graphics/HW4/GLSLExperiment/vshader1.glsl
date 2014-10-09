#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform vec4 LightPosition;

in vec4 vPosition;
in vec4 vNormal;

out vec3 fN;
out vec3 fE;
out vec3 fL;
void main() 
{
	fN = (vNormal).xyz;
	fE = (-(model_matrix*vPosition)).xyz;
	fL = LightPosition.xyz;

	if(LightPosition.w != 0.0){
		fL = LightPosition.xyz - vPosition.xyz;
	}

	gl_Position = projection_matrix*model_matrix*vPosition;

} 
