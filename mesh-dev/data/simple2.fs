#version 150

in vec3 vColor;
out vec4 outColor;

float rand(vec2 co)
{
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	float a, b, c;
	a = rand(vColor.xy);
	b = rand(vColor.yz);
	c = rand(vColor.zx);
	outColor = vec4(a,b,c, 1.0);
}