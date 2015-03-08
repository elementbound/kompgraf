#include "util.h"
#include <cmath>
#include <fstream>
#include <iostream>

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

buffer& operator<<(buffer& b, const glm::vec2& v)
{
	b << v.x << v.y;
	return b;
}

buffer& operator<<(buffer& b, const glm::vec3& v)
{
	b << v.x << v.y << v.z;
	return b;
}

double fac(int x)
{
    static double* cache = NULL;
    static size_t cache_size = 0;
    static const size_t cache_step = 32;

    if(x <= 0)
        return 1;

    if(x >= int(cache_size))
    {
        delete [] cache;
        cache_size = (x/cache_step + 2)*cache_step;
        cache = new double[cache_size];
        cache[0] = 1.0;

        std::cout << "[fac]Freeing cache, recalculating till " << cache_size << std::endl;

        for(unsigned i=1; i<cache_size; i++)
            cache[i] = cache[i-1]*i;
    }

    return cache[x];
}

double combi(int n, int k)
{
    return fac(n) / (fac(k)*fac(n-k));
}