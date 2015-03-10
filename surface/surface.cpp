#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/io.hpp>
#include "frame/util.h"
#include "frame/window.h"
#include "frame/shader.h"
#include "frame/mesh.h"

#include "draw_circles.h"
#include "editable_control_poly.h"
#include "spline_surface.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <cmath>

#define DEBUG(msg) std::cout << msg << std::endl

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
}

class window_surface: public window
{
	private:
		shader_program m_DiffuseShader;
		shader_program m_WireShader;
		
		double		m_Aspect;
		glm::mat4	m_View;
		glm::mat4	m_Perspective;
		glm::mat4	m_Ortho;
		glm::vec2	m_Mouse;
		glm::vec4 	m_Viewport;
		
		glm::vec3	m_CameraAt;
		glm::vec2	m_CameraRot;
		float		m_CameraDst;
		bool		m_CameraGrabbed = 0;
		glm::vec2	m_CameraGrabAt;
		
		int m_Width, m_Height;
		
		editable_control_surface m_ControlSurface;
		bezier_surface	m_BezierSurface;
		bool 			m_Editing = 0;
		unsigned		m_FullQuality = 32;
		unsigned		m_EditQuality = 8;
		unsigned		m_GridDensity = 8;
		
		void build_surface(unsigned detail, unsigned grid_density)
		{
			m_BezierSurface.control_data = m_ControlSurface;
			
			m_ControlSurface.build_mesh();
			m_BezierSurface.build_grid(detail, grid_density);
			m_BezierSurface.build_eval(detail);
			
			m_DiffuseShader.use();
			m_BezierSurface.eval_mesh().bind();
			
			m_WireShader.use();
			m_ControlSurface.mesh().bind();
			m_BezierSurface.grid_mesh().bind();
		}
		
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
			m_DiffuseShader.create();
			m_WireShader.create();
			
			if(!m_DiffuseShader.attach(read_file("data/dirlit.vs").c_str(), shader_program::shader_type::vertex))
				return;
			if(!m_DiffuseShader.attach(read_file("data/dirlit.fs").c_str(), shader_program::shader_type::fragment))
				return;
			
			if(!m_WireShader.attach(read_file("data/wireframe.vs").c_str(), shader_program::shader_type::vertex))
				return;
			if(!m_WireShader.attach(read_file("data/wireframe.fs").c_str(), shader_program::shader_type::fragment))
				return;
			
			m_DiffuseShader.link();
			m_WireShader.link();
			glBindFragDataLocation(m_DiffuseShader.handle(), 0, "outColor");
			glBindFragDataLocation(m_WireShader.handle(), 0, "outColor");
			
			//Control surface
			m_ControlSurface.resize(4,4);
			for(unsigned row = 0; row < m_ControlSurface.rows(); row++)
				for(unsigned col = 0; col < m_ControlSurface.columns(); col++)
				{
					float u, v, z;
					u = col/(float)(m_ControlSurface.columns()-1);
					v = row/(float)(m_ControlSurface.rows()-1);
					
					u = (u-0.5f)*2.0f;
					v = (v-0.5f)*2.0f;
					z = glm::perlin(glm::vec2(u,v)) * 2.0f;
					//z = std::pow(1.0f-std::abs(u), 2) * std::pow(1.0f-std::abs(v), 2) * 2.0f;
					
					m_ControlSurface(row,col) = glm::vec3(u*2.0f, v*2.0f, z);
				}
			build_surface(m_FullQuality, m_GridDensity);
			
			m_CameraAt = glm::vec3(4.0f,4.0f,4.0f);
			m_CameraRot = glm::vec2(glm::radians(45.0f), glm::radians(45.0f));
			m_CameraDst = 6.0f;
		}
		
		void on_fbresize(int w, int h)
		{
			m_Width = w;
			m_Height = h;
			
			m_Aspect = (double)m_Width / m_Height;
			glViewport(0,0, w,h);
			
			m_Perspective = glm::perspective(glm::radians(60.0f), (float)m_Aspect, 1.0f/64.0f, 64.0f);
			m_Ortho = glm::ortho(0.0f,(float)w, 0.0f,(float)h, -1.0f, 1.0f);
			m_Viewport = glm::vec4(0.0f, 0.0f, (float)m_Width, (float)m_Height);
		}
		
		void on_mousebutton(int button, int action, int mods)
		{
			if(button == GLFW_MOUSE_BUTTON_RIGHT && !m_Editing)
			{
				if(action == GLFW_PRESS)
				{
					m_CameraGrabbed = 1;
					glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
					m_CameraGrabAt = m_Mouse;
				}
				else if(action == GLFW_RELEASE)
				{
					m_CameraGrabbed = 0;
					glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				}
			}
			
			if(button == GLFW_MOUSE_BUTTON_LEFT && !m_CameraGrabbed)
			{
				if(action == GLFW_PRESS)
				{
					int grab = m_ControlSurface.grab(m_Mouse, m_View, m_Perspective, m_Viewport);
					if(grab >= 0)
					{
						glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						m_Editing = 1;
					}
				}
				else if(action == GLFW_RELEASE)
				{
					if(m_Editing)
						build_surface(m_FullQuality, m_GridDensity);
					
					m_ControlSurface.ungrab();
					glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					m_Editing = 0;
				}
			}
		}
		
		void on_mousepos(double x, double y)
		{
			m_Mouse = glm::vec2(x,m_Height - y);
			if(m_CameraGrabbed)
			{
				glm::vec2 delta = m_CameraGrabAt - m_Mouse;
				m_CameraRot.x = glm::clamp(m_CameraRot.x + delta.y / 64.0f, -glm::radians(89.0f), glm::radians(89.0f));
				m_CameraRot.y = std::fmod(m_CameraRot.y - delta.x / 64.0f, glm::two_pi<float>());
				
				m_CameraGrabAt = m_Mouse;
			}
		}
		
		void on_mousescroll(double x, double y)
		{
			if(y > 0.0)
				m_CameraDst *= std::pow(2.0, 1.0/8.0);
			else 
				m_CameraDst /= std::pow(2.0, 1.0/8.0);
		}
		
		void on_refresh()
		{
			//Update
			if(glfwGetKey(this->handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(this->handle(), 1);
			
			if(m_Editing)
			{
				m_ControlSurface.edit(m_Mouse, m_View, m_Perspective, m_Viewport);
				build_surface(m_EditQuality, m_GridDensity);
			}
			
			//Draw
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			m_CameraAt = dirvec(m_CameraRot.y, m_CameraRot.x) * m_CameraDst;
			m_View = glm::lookAt(m_CameraAt, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f));
			
			//
			
			glEnable(GL_DEPTH_TEST);
			
			m_DiffuseShader.use();
			m_DiffuseShader.set_uniform("uModelView", m_View); 
			m_DiffuseShader.set_uniform("uProjection", m_Perspective); 
			m_DiffuseShader.set_uniform("uLightDir", glm::vec3(0.707f, 0.707f, 0.0f)); 
			m_BezierSurface.eval_mesh().draw();
			
			//
			
			m_WireShader.use();
			m_WireShader.set_uniform("uMVP", m_Perspective * m_View); 
			
			glDisable(GL_DEPTH_TEST);
			m_WireShader.set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.25f));
			m_ControlSurface.mesh().draw();
			m_BezierSurface.grid_mesh().draw();
			
			glEnable(GL_DEPTH_TEST);
			m_WireShader.set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
			m_ControlSurface.mesh().draw();
			m_BezierSurface.grid_mesh().draw();
			
			//
			
			glDisable(GL_DEPTH_TEST);
			m_WireShader.set_uniform("uMVP", m_Ortho);
			m_WireShader.set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			m_ControlSurface.draw_grabs(m_View, m_Perspective, m_Viewport);
			
			//
			
			glfwSwapBuffers(this->handle());
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
		return 132;
	
	glfwSwapInterval(0);
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}