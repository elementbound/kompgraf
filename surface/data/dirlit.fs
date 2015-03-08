#version 330

uniform vec3 uLightDir;

in vec3 fragNormal;
out vec4 outColor;

void main()
{
	vec3 normal = normalize(fragNormal);
	float d = dot(normal, uLightDir);
	outColor = vec4(d,d,d, 1.0);
}