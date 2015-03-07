#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "frame/window.h"
#include "frame/shader.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iterator> //istream_iterator
#include <cmath>

double degtorad(double deg){return deg/180.0*3.14159265;}
float degtorad(float deg){return deg/180.0f*3.14159265f;}

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
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

#include "frame/buffer.hpp"
#include <map>

class basic_mesh
{
	public: 
		struct stream_data
		{
			unsigned index; 
			GLenum type;
			GLenum buffer_type;
			unsigned components;
			bool normalized;
			
			buffer data;
		};
		
		//TODO: implement
		basic_mesh();
		basic_mesh(const basic_mesh&) = delete;
		~basic_mesh();
		
		basic_mesh& operator=(const basic_mesh&) = delete;
	
		unsigned add_stream();
		bool     remove_stream(unsigned id);
		
		stream_data& get_stream(unsigned id);
		const stream_data& get_stream(unsigned id) const;
		stream_data& operator[](unsigned id);
		const stream_data& operator[](unsigned id) const;
		
		std::vector<stream_data> streams;
		virtual void upload() = 0;
		virtual void bind() = 0;
		virtual void draw() = 0;
		
	protected: 
		std::map<unsigned, stream_data> m_Streams;
		GLuint m_VAO;
};

basic_mesh::basic_mesh()
{
	glGenVertexArrays(1, &m_VAO);
	std::cout << "Acquired VAO#" << m_VAO << std::endl;
}

basic_mesh::~basic_mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	std::cout << "Released VAO#" << m_VAO << std::endl;
}

unsigned basic_mesh::add_stream()
{
	unsigned nid = 0;
	if(!m_Streams.empty())
		nid = m_Streams.rbegin()->first+1;
		
	m_Streams.insert({nid, stream_data()});
	return nid;
}

bool basic_mesh::remove_stream(unsigned id)
{
	return m_Streams.erase(id);
}

basic_mesh::stream_data& basic_mesh::get_stream(unsigned id)
{
	return m_Streams.at(id);
}

const basic_mesh::stream_data& basic_mesh::get_stream(unsigned id) const
{
	return m_Streams.at(id);
}

basic_mesh::stream_data& basic_mesh::operator[](unsigned id)
{
	return this->get_stream(id);
}

const basic_mesh::stream_data& basic_mesh::operator[](unsigned id) const 
{
	return this->get_stream(id);
}

class separated_mesh : public basic_mesh
{
	private: 
		std::map<unsigned, GLuint> m_VBOs;
		
	public: 
		separated_mesh() = default;
		separated_mesh(const separated_mesh&) = default;
		~separated_mesh();
	
		void upload();
		void bind();
		void draw();
};

void separated_mesh::upload()
{
	for(std::pair<const unsigned, stream_data>& p: m_Streams)
	{
		if(!m_VBOs.count(p.first))
		{
			GLuint id;
			glGenBuffers(1, &id);
			m_VBOs.insert({p.first, id});
		}
		
		glBindBuffer(p.second.buffer_type, m_VBOs[p.first]);
		glBufferData(p.second.buffer_type, p.second.data.size(), p.second.data.data(), GL_STATIC_DRAW);
	}
}

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

class window_triangle: public window
{
	private:
		std::vector<float> vertices;
		std::vector<float> colors;
		GLuint vbo;
		GLuint vbo_color;
		GLuint vao;
		shader_program program;
		
	protected: 
		void on_open()
		{
			this->make_current();
			
			if(glewInit() != GLEW_OK)
			{
				std::cerr << "GLEW init fail" << std::endl;
				return;
			}
		
			if(!GLEW_VERSION_3_3)
			{
				std::cerr << "OpenGL 3.3 not supported" << std::endl;
				return; 
			}
		
			vertices = {
				std::cos(degtorad(  0.0f)), std::sin(degtorad(  0.0f)), 
				std::cos(degtorad(120.0f)), std::sin(degtorad(120.0f)), 
				std::cos(degtorad(240.0f)), std::sin(degtorad(240.0f)) 
			};
			
			colors = {
				1.0, 0.0, 0.0, 
				0.0, 1.0, 0.0, 
				0.0, 0.0, 1.0
			};
			
			std::cout << "Creating VBOs... ";
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
			
			glGenBuffers(1, &vbo_color);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*colors.size(), colors.data(), GL_STATIC_DRAW);
			std::cout << "Done" << std::endl;
			
			//Shaders
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			
			std::cout << "Compiling shaders... ";
			program.create();
			
			if(!program.attach(read_file("data/simple2.vs").c_str(), shader_program::shader_type::vertex))
			{
				std::cerr << "Couldn't attach vertex shader" << std::endl;
				return;
			}
			
			if(!program.attach(read_file("data/simple2.fs").c_str(), shader_program::shader_type::fragment))
			{
				std::cerr << "Couldn't attach fragment shader" << std::endl;
				return;
			}
			
			glBindFragDataLocation(program.handle(), 0, "outColor");
			program.link();
			program.use();
			
			//Link attribs
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			GLint attrib_pos = glGetAttribLocation(program.handle(), "inPosition");
			glEnableVertexAttribArray(attrib_pos);
			glVertexAttribPointer(attrib_pos, 2, GL_FLOAT, 0, 0, NULL);
			
			glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
			GLint attrib_col = glGetAttribLocation(program.handle(), "inColor");
			glEnableVertexAttribArray(attrib_col);
			glVertexAttribPointer(attrib_col, 3, GL_FLOAT, 0, 0, NULL);
			
			std::cout << "Ready to use" << std::endl;
		}
		
		void on_refresh()
		{
			static float f = 0.0;
			f += 1.0/256.0;
			if(f >= 1.0) f -= 2.0f;
			
			glClear(GL_COLOR_BUFFER_BIT);
			
			float angle = fmod(glfwGetTime()/4.0, 1.0) * 2.0 * 3.14159265;
			glm::mat4 matRot;
			matRot = glm::rotate(matRot, angle, glm::vec3(0.0, 0.0, 1.0));
			int u = glGetUniformLocation(program.handle(), "uMVP");
			glUniformMatrix4fv(u, 1, 0, glm::value_ptr(matRot));
			glDrawArrays(GL_TRIANGLES, 0, 3);
			
			glfwSwapBuffers(this->handle());
			
			if(glfwGetKey(this->handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(this->handle(), 1);
		}
		
		void on_close()
		{
			glDeleteBuffers(1, &vbo);
		}
};

int main()
{
	glfwSetErrorCallback(error_callback);
	if(!glfwInit())
		return 1;
		
	window_triangle wnd;
	
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	wnd.open(512,512, "it's a tRIANGLE");
	if(!wnd)
		return 3;
	
	wnd.make_current();
	
	std::cout << "Init done, starting loop" << std::endl;
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}