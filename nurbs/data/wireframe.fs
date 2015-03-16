#version 330

uniform vec4 uColor;

in	float	fragmentDistance;
out vec4 	outColor;

void main()
{
	outColor = uColor + vec4(vec3(1.0, 1.0, 1.0) * fract(fragmentDistance / 64.0), 1.0);
}