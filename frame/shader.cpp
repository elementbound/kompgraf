#include "shader.h"
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

GLuint shader_program::handle()
{
	return m_Program;
}