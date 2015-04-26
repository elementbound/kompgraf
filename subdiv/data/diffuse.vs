#version 330

uniform mat4 uModelView;
uniform mat4 uProjection;

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 fragNormal;

void main()
{
	gl_Position = uProjection * uModelView * vec4(vertexPosition, 1.0);
	fragNormal = (uModelView * vec4(vertexNormal, 0.0)).xyz;
}