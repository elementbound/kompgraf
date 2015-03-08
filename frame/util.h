#ifndef _H_UTIL_H_
#define _H_UTIL_H_

#include <string>
#include <GL/glew.h>

std::string read_file(const std::string& fname);
const char* gl_error_str(GLenum err);

#endif