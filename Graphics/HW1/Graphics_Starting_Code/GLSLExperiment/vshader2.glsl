#version 150
uniform mat4 Proj;
in vec4 vColor;
in vec4 vPosition;
out vec4 color;
void
main()
{
    gl_Position = Proj * vPosition;
	color = vColor;
}
