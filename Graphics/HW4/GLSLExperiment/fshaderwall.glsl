#version 150

uniform vec4 color;
uniform bool useTex;
uniform sampler2D texture;
in vec2 texCoord;
out vec4 fColor;

void main() 
{ 

	if(useTex){
		fColor = texture2D( texture, texCoord);
	} else {
		fColor = color;	
	}

     
} 

