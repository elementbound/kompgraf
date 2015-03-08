#include "util.h"
#include <cmath>
#include <fstream>

std::string read_file(const std::string& fname)
{
	std::string ret = "";
	std::string line;
	std::ifstream file(fname);
	
	while(std::getline(file, line))
	{
		ret.append(line);
		ret.push_back('\n');
	}
		
	return ret;
}

const char* gl_error_str(GLenum err)
{
	switch(err)
	{
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		default: 
			return "Say wat?";
	}
}

glm::vec2 dirvec(float dir)
{
	glm::vec2 ret;
	ret.x =  std::cos(dir);
	ret.y = -std::sin(dir);
	return ret;
}

glm::vec3 dirvec(float dir, float pitch)
{
	glm::vec3 ret;
	ret.x =  std::cos(dir) * std::cos(pitch);
	ret.y = -std::sin(dir) * std::cos(pitch);
	ret.z =  std::sin(pitch);
	return ret;
}

buffer& operator<<(buffer& b, const glm::vec3& v)
{
	b << v.x << v.y << v.z;
	return b;
}