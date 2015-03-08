#include "shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void shader_program::shader::source(const char* src, shader_program::shader_type type)
{
	if(gl_id != 0)
		glDeleteShader(gl_id);
		
	switch(type)
	{
		case shader_program::shader_type::vertex: 
			gl_id = glCreateShader(GL_VERTEX_SHADER);
			break;
			
		case shader_program::shader_type::fragment: 
			gl_id = glCreateShader(GL_FRAGMENT_SHADER);
			break;
			
		case shader_program::shader_type::geometry:
			gl_id = glCreateShader(GL_GEOMETRY_SHADER);
			break;
			
		default:
			std::cerr << "Invalid shader type" << std::endl;
			return;
	}
	
	glShaderSource(gl_id, 1, &src, NULL);
}

bool shader_program::shader::compile()
{
	glCompileShader(gl_id);
	
	int status;
	glGetShaderiv(gl_id, GL_COMPILE_STATUS, &status);
	
	return status == GL_TRUE;
}

std::string shader_program::shader::log()
{
	int size;
	glGetShaderiv(gl_id, GL_INFO_LOG_LENGTH, &size);
	
	if(size == 0) 
		return std::string();
		
	char buff[size];
	
	glGetShaderInfoLog(gl_id, size, NULL, buff);
	return std::string(buff);
}

void shader_program::create()
{
	m_Program = glCreateProgram();
}

bool shader_program::attach(const char* src, shader_type type)
{
	shader np;
	np.source(src, type);
	if(!np.compile())
	{
		std::cerr << "Couldn't compile shader, compile log: \n" << np.log() << "\n"
				  << "Shader source: \n" << src << std::endl;
		return 0;
	}
	
	m_Shaders.push_back(np);
	glAttachShader(m_Program, np.gl_id);
	return 1;
}


void shader_program::link()
{
	glLinkProgram(m_Program);
}

void shader_program::use()
{
	glUseProgram(m_Program);
}

GLuint shader_program::handle() const
{
	return m_Program;
}

int  shader_program::uniform(const char* name) const
{
	return glGetUniformLocation(this->handle(), name);
}

int  shader_program::uniform(const std::string& name) const
{
	return glGetUniformLocation(this->handle(), name.c_str());
}

void shader_program::set_uniform(int loc, glm::vec2 value) const
{
	glUniform2f(loc, value.x, value.y);
}

void shader_program::set_uniform(int loc, glm::vec3 value) const
{
	glUniform3f(loc, value.x, value.y, value.z);
}

void shader_program::set_uniform(int loc, glm::vec4 value) const
{
	glUniform4f(loc, value.x, value.y, value.z, value.w);
}

void shader_program::set_uniform(int loc, glm::mat4 value) const
{
	glUniformMatrix4fv(loc, 1, 0, glm::value_ptr(value));
}

#define SET_UNIFORM_MACRO(name, value) \
	int loc = this->uniform(name); \
	if(loc < 0) \
		std::cerr << "Couldn't find uniform called " << name << std::endl; \
	else \
		this->set_uniform(loc, value); 

void shader_program::set_uniform(const char* name, glm::vec2 value) const
{
	SET_UNIFORM_MACRO(name, value)
}

void shader_program::set_uniform(const char* name, glm::vec3 value) const
{
	SET_UNIFORM_MACRO(name, value)
}

void shader_program::set_uniform(const char* name, glm::vec4 value) const
{
	SET_UNIFORM_MACRO(name, value)
}

void shader_program::set_uniform(const char* name, glm::mat4 value) const
{
	SET_UNIFORM_MACRO(name, value)
}

//

void shader_program::set_uniform(const std::string& name, glm::vec2 value) const
{
	SET_UNIFORM_MACRO(name.c_str(), value)
}

void shader_program::set_uniform(const std::string& name, glm::vec3 value) const
{
	SET_UNIFORM_MACRO(name.c_str(), value)
}

void shader_program::set_uniform(const std::string& name, glm::vec4 value) const
{
	SET_UNIFORM_MACRO(name.c_str(), value)
}

void shader_program::set_uniform(const std::string& name, glm::mat4 value) const
{
	SET_UNIFORM_MACRO(name.c_str(), value)
}

#undef SET_UNIFORM_MACRO