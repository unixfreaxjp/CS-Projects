#version 150

uniform bool useColor;
uniform vec4 color;
in  vec4  interpolatedColor;
out vec4  fColor;

void main() 
{ 
	if(useColor){	
		fColor = interpolatedColor;
	} else {
		fColor = color;
	}
     
} 

