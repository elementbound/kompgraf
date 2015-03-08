#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "frame/util.h"
#include "frame/window.h"
#include "frame/shader.h"
#include "frame/mesh.h"

#include <iostream>
#include <string>
#include <vector>
#include <cmath>

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
}

class editable_poly
{
	private: 
		std::vector<glm::vec3> 	m_PointMatrix;
		unsigned 				m_PointRows;
		unsigned 				m_PointColumns;
		
		separated_mesh			m_EvalMesh;
		separated_mesh			m_ControlMesh;
		separated_mesh			m_WireMesh;
		
	public: 
		shader*	diffuse_shader; 
		shader* wireframe_shader;
		
		void resize(unsigned rows, unsigned cols, float spacing, float height)
		{
			m_PointRows = rows;
			m_PointColumns = cols;
			
			m_PointMatrix.resize(m_PointRows * m_PointColumns);
			for(unsigned y = 0; y < m_PointRows; y++)
			{
				for(unsigned x = 0; x < m_PointColumns; x++)
				{
					float u, v, z;
					u = x/(float)m_PointColumns;
					v = y/(float)m_PointRows;
					
					u = (u-0.5f)*2.0f;
					v = (v-0.5f)*2.0f;
					z = std::pow(u, 2) * std::pow(v, 2) * height;
					
					m_PointMatrix[y*m_PointRows + x] = glm::vec3(u*spacing, v*spacing, z);
				}
			}
		}
		
		void build(unsigned detail)
		{
			
		}
};

class window_surface: public window
{
	private:
		shader_program m_Shader;
		
		double		m_Aspect;
		glm::mat4	m_Projection;
		glm::vec2	m_Mouse;
		
		int m_Width, m_Height;
		
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
			
			int w, h;
			glfwGetFramebufferSize(this->handle(), &w, &h);
			this->on_fbresize(w,h);
			
			glfwSetInputMode(this->handle(), GLFW_STICKY_KEYS, GL_TRUE);
			glfwSetInputMode(this->handle(), GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
			
			//Shaders
			std::cout << "Compiling shaders... ";
			m_Shader.create();
			
			if(!m_Shader.attach(read_file("data/dirlit.vs").c_str(), shader_program::shader_type::vertex))
			{
				std::cerr << "Couldn't attach vertex shader" << std::endl;
				return;
			}
			
			if(!m_Shader.attach(read_file("data/dirlit.fs").c_str(), shader_program::shader_type::fragment))
			{
				std::cerr << "Couldn't attach fragment shader" << std::endl;
				return;
			}
			
			glBindFragDataLocation(m_Shader.handle(), 0, "outColor");
			m_Shader.link();
			m_Shader.use();
			
			std::cout << "Ready to use" << std::endl;
		}
		
		void on_fbresize(int w, int h)
		{
			m_Width = w;
			m_Height = h;
			
			m_Aspect = (double)m_Width / m_Height;
			glViewport(0,0, w,h);
			
			glMatrixMode(GL_PROJECTION);
			m_Projection = glm::ortho(-m_Aspect,m_Aspect, -1.0,1.0);
			glLoadMatrixf(glm::value_ptr(m_Projection));
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
			int u = glGetUniformLocation(m_Shader.handle(), "uMVP");
			glUniformMatrix4fv(u, 1, 0, glm::value_ptr(matRot));
			
			glfwSwapBuffers(this->handle());
			
			if(glfwGetKey(this->handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(this->handle(), 1);
		}
};

int main()
{
	glfwSetErrorCallback(error_callback);
	if(!glfwInit())
		return 1;
		
	window_surface wnd;
	
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	wnd.open(640,480, "gone GRID");
	if(!wnd)
		return 3;
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}