#version 330

uniform mat4 uMVP;

in  vec3 vertexPosition;

void main()
{
	gl_Position = uMVP * vec4(vertexPosition, 1.0);
}