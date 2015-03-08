#ifndef _H_UTIL_H_
#define _H_UTIL_H_

#include <string>
#include <GLM/glm.hpp>
#include <GL/glew.h>
#include "buffer.hpp"

std::string read_file(const std::string& fname);
const char* gl_error_str(GLenum err);

glm::vec2 dirvec(float dir);
glm::vec3 dirvec(float dir, float pitch);

buffer& operator<<(buffer&, const glm::vec3&);

#endif