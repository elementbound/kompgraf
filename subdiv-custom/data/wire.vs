#version 330

uniform mat4 uMVP;

in  vec3 	vertexPosition;
in	float	vertexDistance;

out float	fragmentDistance;

void main()
{
	fragmentDistance = vertexDistance;
	gl_Position = uMVP * vec4(vertexPosition, 1.0);
}