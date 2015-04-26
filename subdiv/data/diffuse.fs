#version 330

uniform vec3 uLightDir;

in vec3 fragNormal;
out vec4 outColor;

void main()
{
	vec3 normal = normalize(fragNormal);
	float d = (1.0 + dot(normal, uLightDir))/2.0;
	//outColor = vec4((1.0+normal)/2.0, 1.0);
	outColor = vec4(d,d,d, 1.0);
}