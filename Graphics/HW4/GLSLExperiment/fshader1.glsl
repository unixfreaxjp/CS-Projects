#version 150

uniform vec4 color;
uniform vec4 ambient, diffuse, specular;
uniform vec4 LightPosition;
uniform float shininess;

uniform bool shadow;

uniform int mode;
uniform samplerCube cube;

in vec3 R;
in vec3 T;

in vec3 fN;
in vec3 fL;
in vec3 fE;

out vec4 fColor;

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

	vec4 phongColor = ambient + diffuseColor + specularColor;
	vec4 reflectColor = textureCube(cube, R);
	vec4 refractColor = textureCube(cube, T);

	if(mode == 1){
		fColor = phongColor;
	} else if(mode == 2){
		fColor = reflectColor;
	} else if (mode == 3) {
		fColor = mix(refractColor,specularColor+vec4(0.3,0.3,0.3,1), 0.1);
	} else {
		fColor = mix(reflectColor, refractColor, 0.5);
	}
	
	fColor.a = 1.0;

	if(shadow){
		fColor = vec4(0,0,0,0.5);
	}

     
} 

