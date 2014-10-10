#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform vec4 LightPosition;

in vec4 vPosition;
in vec4 vNormal;

out vec3 fN;
out vec3 fE;
out vec3 fL;
out vec3 T;
out vec3 R;
void main() 
{
	fN = (vNormal).xyz;
	fE = (-(model_matrix*vPosition)).xyz;
	fL = LightPosition.xyz;

	if(LightPosition.w != 0.0){
		fL = LightPosition.xyz - vPosition.xyz;
	}
	vec3 NN = normalize((model_matrix*vNormal).xyz);
	R = reflect(vPosition.xyz, NN);
	gl_Position = projection_matrix*model_matrix*vPosition;

} 
