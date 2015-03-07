#version 150

uniform mat4 uMVP;

in vec2 inPosition; 
in vec3 inColor;

out vec3 vColor;

void main()
{
	gl_Position = uMVP * vec4(inPosition, 0.0, 1.0);
	vColor = inColor;
}