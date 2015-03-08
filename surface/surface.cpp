#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
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
		
		unsigned hash(unsigned x, unsigned y) const {
			return y*m_PointColumns + x;
		}
		
	public: 
		shader_program*	diffuse_shader; 
		shader_program* wireframe_shader;
		
		void resize(unsigned rows, unsigned cols, float size, float height)
		{
			m_PointRows = rows;
			m_PointColumns = cols;
			
			m_PointMatrix.resize(m_PointRows * m_PointColumns);
			for(unsigned y = 0; y < m_PointRows; y++)
			{
				for(unsigned x = 0; x < m_PointColumns; x++)
				{
					float u, v, z;
					u = x/(float)(m_PointColumns-1);
					v = y/(float)(m_PointRows-1);
					
					u = (u-0.5f)*2.0f;
					v = (v-0.5f)*2.0f;
					z = std::pow(1.0f-std::abs(u), 2) * std::pow(1.0f-std::abs(v), 2) * height;
					
					m_PointMatrix[hash(x,y)] = glm::vec3(u*size, v*size, z);
				}
			}
		}
		
		void build(unsigned detail)
		{
			m_WireMesh.clear_streams();
			m_WireMesh.storage_policy = GL_DYNAMIC_DRAW;
			m_WireMesh.draw_mode = GL_LINES;
			
			unsigned pos = m_WireMesh.add_stream();
			m_WireMesh[pos].type = GL_FLOAT;
			m_WireMesh[pos].buffer_type = GL_ARRAY_BUFFER;
			m_WireMesh[pos].components = 3;
			m_WireMesh[pos].normalized = 0;
			m_WireMesh[pos].name = "vertexPosition";
			
			for(unsigned y=0; y<m_PointRows; y++)
			{
				for(unsigned x=0; x<m_PointColumns; x++)
				{
					if(x+1 < m_PointColumns)
					{
						m_WireMesh[pos].data << m_PointMatrix[hash(x  , y)];
						m_WireMesh[pos].data << m_PointMatrix[hash(x+1, y)];
					}
					
					if(y+1 < m_PointRows)
					{
						m_WireMesh[pos].data << m_PointMatrix[hash(x, y  )];
						m_WireMesh[pos].data << m_PointMatrix[hash(x, y+1)];
					}
				}
			}
			
			m_WireMesh.upload();
			
			if(wireframe_shader != NULL)
			{
				wireframe_shader->use();
				m_WireMesh.bind();
			}
			
			build_bezier(detail);
		}
		
		void build_bezier(unsigned detail)
		{
		};
		
		void draw(glm::mat4 matVP)
		{
			if(wireframe_shader != NULL)
			{
				wireframe_shader->use();
				glUniformMatrix4fv(glGetUniformLocation(wireframe_shader->handle(), "uMVP"), 1, 0, glm::value_ptr(matVP));
				m_WireMesh.draw();
			}
		}
};

class window_surface: public window
{
	private:
		shader_program m_DiffuseShader;
		shader_program m_WireShader;
		
		double		m_Aspect;
		glm::mat4	m_View;
		glm::mat4	m_Projection;
		glm::vec2	m_Mouse;
		glm::vec3	m_CameraAt;
		
		int m_Width, m_Height;
		
		editable_poly	m_Poly;
		
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
			
			glClearColor(1.0, 1.0, 1.0, 1.0);
			
			//Shaders
			std::cout << "Compiling shaders... ";
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
			
			glBindFragDataLocation(m_DiffuseShader.handle(), 0, "outColor");
			glBindFragDataLocation(m_WireShader.handle(), 0, "outColor");
			m_DiffuseShader.link();
			m_WireShader.link();
			
			std::cout << "Ready to use" << std::endl;
			
			m_Poly.wireframe_shader = &m_WireShader;
			m_Poly.diffuse_shader = &m_DiffuseShader;
			m_Poly.resize(4,4, 2.0, 2.0);
			
			m_Poly.build(512);
			
			m_CameraAt = glm::vec3(4.0f,4.0f,4.0f);
		}
		
		void on_fbresize(int w, int h)
		{
			m_Width = w;
			m_Height = h;
			
			m_Aspect = (double)m_Width / m_Height;
			glViewport(0,0, w,h);
			
			glMatrixMode(GL_PROJECTION);
			m_Projection = glm::perspective(glm::radians(60.0f), (float)m_Aspect, 1.0f/64.0f, 64.0f);
			glLoadMatrixf(glm::value_ptr(m_Projection));
		}
		
		void on_refresh()
		{
			static float f = 0.0;
			static float angle = 0.0;
			f = std::fmod(glfwGetTime(), 8.0f)/8.0f;
			angle = glm::radians(f*360.0f);
			
			glClear(GL_COLOR_BUFFER_BIT);
			
			m_CameraAt = dirvec(angle, glm::radians(45.0f))*4.0f;
			m_View = glm::lookAt(m_CameraAt, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f));
			
			m_Poly.draw(m_Projection * m_View);
			
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