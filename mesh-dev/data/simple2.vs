#version 330

uniform mat4 uMVP;

in vec2 vertexPosition; 
in vec3 vertexColor;

out vec3 fragColor;

void main()
{
	gl_Position = uMVP * vec4(vertexPosition, 0.0, 1.0);
	fragColor = vertexColor;
}