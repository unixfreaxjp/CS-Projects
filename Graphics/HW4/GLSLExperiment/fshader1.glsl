#version 150

uniform vec4 color;
uniform vec4 ambient, diffuse, specular;
uniform vec4 LightPosition;
uniform float shininess;
out vec4 fColor;

in vec3 fN;
in vec3 fL;
in vec3 fE;

void main() 
{ 
	vec3 N = normalize(fN);
	vec3 E = normalize(fE);
	vec3 L = normalize(fL);

	vec3 H = normalize(L + E);

	float kd = max(dot(L, N), 0.0);
	vec4 diffuseColor = kd*diffuse;

	float ks = pow(max(dot(N, H), 0.0), shininess);
	vec4 specularColor = ks*specular;

	if(dot(L, N) < 0.0){
		specularColor = vec4(0.0, 0.0, 0.0, 1.0);
	}

	fColor = ambient + diffuseColor + specularColor;
	fColor.a = 1.0;

	//fColor.xyz = N;
	//fColor.a = 1.0;
     
} 

