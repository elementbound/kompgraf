#version 330

uniform mat4 uMVP;

layout(location=0) in vec2 inPosition; 
layout(location=1) in vec3 inColor;

out vec3 vColor;

void main()
{
	gl_Position = uMVP * vec4(inPosition, 0.0, 1.0);
	vColor = inColor;
}