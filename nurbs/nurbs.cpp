#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>

#include "glwrap/util.h"
#include "glwrap/resizable_window.h"
#include "glwrap/shader.h"
#include "glwrap/mesh.h"

#include "editable_control_poly.h"
#include "spline_poly.h"

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
		editable_control_poly	m_ControlPoly;
		nurbs_poly				m_SplinePoly;
		
		glm::mat4	m_View;
		glm::mat4	m_Ortho;
		glm::vec2	m_Mouse;
		glm::vec4 	m_Viewport;
		
		bool m_Editing = 0;
		unsigned m_FullQuality = 16;
		unsigned m_EditQuality = 64;
		int m_GrabId = -1;
		
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
			
			glfwSetInputMode(this->handle(), GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
			
			//GL init
			glClearColor(1.0, 1.0, 1.0, 1.0);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			m_View = glm::mat4(1.0f);
		}
		
		void rebuild_spline()
		{
			m_ControlPoly.build_meshes();
			m_SplinePoly.build_eval(m_Editing ? m_EditQuality : m_FullQuality);
				
			m_WireShader.use();
			m_ControlPoly.control_mesh().bind();
			m_ControlPoly.knot_mesh().bind();
			m_SplinePoly.eval_mesh().bind();
		}
		
	protected: 
		void on_open()
		{
			this->make_current();
			if(!init_glew())
			{
				std::cerr << "GLEW init fail\n";
				return;
			}
			
			init_window();
			
			//Shaders
			m_WireShader.create();
			
			if(!m_WireShader.attach(read_file("data/wireframe.vs").c_str(), shader_program::shader_type::vertex))
			{
				std::cerr << "Couldn't attach vertex shader\n";
				return;
			}
			if(!m_WireShader.attach(read_file("data/wireframe.fs").c_str(), shader_program::shader_type::fragment))
			{
				std::cerr << "Couldn't attach fragment shader\n";
				return;
			}
			
			m_WireShader.link();
			
			//Spline 
			m_SplinePoly.control_data = &m_ControlPoly;
		}
		
		void on_fbresize(int w, int h)
		{
			resizable_window::on_fbresize(w,h);
			
			w += (w==0);
			h += (h==0);
			
			m_Ortho = glm::ortho(0.0f,(float)w, 0.0f,(float)h, -1.0f, 1.0f);
			m_Viewport = glm::vec4(0.0f, 0.0f, (float)m_FramebufferWidth, (float)m_FramebufferHeight);
		}
		
		void on_mousepos(double x, double y)
		{
			m_Mouse = glm::vec2(x,m_FramebufferHeight - y); //Shady stuff
			
			if(m_Editing) 
			{
				m_ControlPoly.edit(m_Mouse, m_View, m_Ortho, m_Viewport);
				rebuild_spline();
			}
		}
		
		void on_key(int key, int scancode, int action, int mods)
		{
			if(key == GLFW_KEY_Q && !m_Editing && action == GLFW_PRESS)
			{
				glm::vec2 point = m_Mouse; //Screen-space
				point = glm::unProject(glm::vec3(m_Mouse.x, m_Mouse.y, 0.0f), m_View, m_Ortho, m_Viewport).xy(); //World-space 
				m_ControlPoly.add(point);
			}
			
			if(key == GLFW_KEY_W && action == GLFW_PRESS)
			{
				for(unsigned i=0; i<m_ControlPoly.size(); i++)
					m_ControlPoly.weight(i) = 1.0f;
			}
			
			if(key == GLFW_KEY_UP && m_Editing && ( action == GLFW_PRESS || action == GLFW_REPEAT))
				m_ControlPoly.weight(m_GrabId) *= std::pow(2.0, 1.0/16.0);
			
			if(key == GLFW_KEY_DOWN && m_Editing && ( action == GLFW_PRESS || action == GLFW_REPEAT))
				m_ControlPoly.weight(m_GrabId) /= std::pow(2.0, 1.0/16.0);
			
			//Almost any key press involves rebuilding the spline so... 
			rebuild_spline();
		}
		
		void on_mousebutton(int button, int action, int mods)
		{
			if(button == GLFW_MOUSE_BUTTON_LEFT)
			{
				if(action == GLFW_PRESS)
				{
					m_GrabId = m_ControlPoly.grab(m_Mouse, m_View, m_Ortho, m_Viewport);
					m_Editing = (m_GrabId >= 0);
					if(m_Editing) glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				}
				else if(action == GLFW_RELEASE)
				{
					m_Editing = 0;
					m_ControlPoly.ungrab();
					glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}
		}
		
		void on_refresh()
		{
			//Update
			if(glfwGetKey(this->handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(this->handle(), 1);
			
			//Draw
			glClear(GL_COLOR_BUFFER_BIT);
			
			m_WireShader.use();
			m_WireShader.set_uniform("uMVP", m_Ortho * m_View);
			m_WireShader.set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			m_ControlPoly.control_mesh().draw();
			m_ControlPoly.knot_mesh().draw();
			m_SplinePoly.eval_mesh().draw();
			
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
	glfwWindowHint(GLFW_SAMPLES, 8);
	
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