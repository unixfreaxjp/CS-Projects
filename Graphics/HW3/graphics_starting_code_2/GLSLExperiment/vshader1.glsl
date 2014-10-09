#version 150

uniform mat4 projection_matrix;
uniform mat4 model_transform;
uniform mat4 model_matrix;
uniform float shear;
uniform float twist;
in  vec4 vPosition;
in  vec4 vColor;

out vec4 interpolatedColor;

void main() 
{
  gl_Position = projection_matrix*model_matrix*model_transform*vPosition;
  interpolatedColor = vColor;
} 
