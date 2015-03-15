#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "frame/util.h"
#include "frame/resizable_window.h"
#include "frame/shader.h"
#include "frame/mesh.h"

#include "editable_control_poly.h"

#include <iostream> 

#define DEBUG(msg) std::cout << msg << std::endl

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
}

class nurbs_window : public resizable_window
{
	private: 
		shader_program			m_WireShader;
		editable_control_poly	m_Poly;
		
		glm::mat4	m_View;
		glm::mat4	m_Ortho;
		glm::vec2	m_Mouse;
		glm::vec4 	m_Viewport;
		
		bool m_Editing = 0;
		
		bool init_glew()
		{
			if(glewInit() != GLEW_OK)
			{
				std::cerr << "GLEW init fail" << std::endl;
				return 0;
			}
		
			if(!GLEW_VERSION_3_3)
			{
				std::cerr << "OpenGL 3.3 not supported" << std::endl;
				return 0; 
			}
			
			return 1;
		}
		
		void init_window()
		{
			int w, h;
			glfwGetFramebufferSize(this->handle(), &w, &h);
			this->on_fbresize(w,h);
			
			glfwSetInputMode(this->handle(), GLFW_STICKY_KEYS, GL_TRUE);
			glfwSetInputMode(this->handle(), GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
			
			//GL init
			glClearColor(1.0, 1.0, 1.0, 1.0);
			glClearDepth(1.0);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		
	protected: 
		void on_open()
		{
			this->make_current();
			if(!init_glew())
				return;
			
			init_window();
			
			//Shaders
			m_WireShader.create();
			
			if(!m_WireShader.attach(read_file("data/wireframe.vs").c_str(), shader_program::shader_type::vertex))
				return;
			if(!m_WireShader.attach(read_file("data/wireframe.fs").c_str(), shader_program::shader_type::fragment))
				return;
			
			m_WireShader.link();
		}
		
		void on_fbresize(int w, int h)
		{
			w += (w==0);
			h += (h==0);
			
			m_Ortho = glm::ortho(0.0f,(float)w, 0.0f,(float)h, -1.0f, 1.0f);
			m_Viewport = glm::vec4(0.0f, 0.0f, (float)m_FramebufferWidth, (float)m_FramebufferHeight);
		}
		
		void on_mousepos(double x, double y)
		{
			m_Mouse = glm::vec2(x,m_WindowHeight - y);
		}
		
		void on_refresh()
		{
			//Update
			if(glfwGetKey(this->handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(this->handle(), 1);
			
			//Draw
			glClear(GL_COLOR_BUFFER_BIT);
			
			glfwSwapBuffers(this->handle());
		}
};

int main()
{
	glfwSetErrorCallback(error_callback);
	if(!glfwInit())
	{
		std::cerr << "Couldn't init GLFW\n";
		return 1;
	}
	
	nurbs_window wnd;
	
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	wnd.open(640,480, "nSPLINE");
	if(!wnd)
	{
		std::cerr << "Couldn't create window\n";
		return 2;
	}
	
	glfwSwapInterval(0);
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}