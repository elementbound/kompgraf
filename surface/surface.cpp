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
#include <map>
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
		
		glm::vec3 eval_bezier(float u, float v)
		{
			glm::vec3 p = glm::vec3(0.0f);
			
			for(unsigned i=0; i<m_PointRows; i++)
				for(unsigned j=0; j<m_PointColumns; j++)
					p += float(std::pow(u,j)*std::pow(1.0f-u, m_PointColumns-1-j)*combi(m_PointColumns-1, j)) * 
						 float(std::pow(v,i)*std::pow(1.0f-v, m_PointRows-1-i)*combi(m_PointRows-1, i))* m_PointMatrix[hash(j,i)];
						 
			return p;
		}
		
		void build_bezier(unsigned detail)
		{
			m_EvalMesh.clear_streams();
			m_EvalMesh.storage_policy = GL_DYNAMIC_DRAW;
			m_EvalMesh.draw_mode = GL_TRIANGLES;
			
			unsigned pos = m_EvalMesh.add_stream();
			m_EvalMesh[pos].type = GL_FLOAT;
			m_EvalMesh[pos].buffer_type = GL_ARRAY_BUFFER;
			m_EvalMesh[pos].components = 3;
			m_EvalMesh[pos].normalized = 0;
			m_EvalMesh[pos].name = "vertexPosition";
			
			unsigned nor = m_EvalMesh.add_stream();
			m_EvalMesh[nor].type = GL_FLOAT;
			m_EvalMesh[nor].buffer_type = GL_ARRAY_BUFFER;
			m_EvalMesh[nor].components = 3;
			m_EvalMesh[nor].normalized = 0;
			m_EvalMesh[nor].name = "vertexNormal";
			
			float nabla = 1.0f/float(detail-1);
			static std::map<std::pair<unsigned,unsigned>, glm::vec3> pos_map;
			static std::map<std::pair<unsigned,unsigned>, glm::vec3> nor_map; //normals
			
			for(unsigned y=0; y<detail; y++)
			{
				for(unsigned x=0; x<detail; x++)
				{
					float u = x/float(detail-1);
					float v = y/float(detail-1);
								 
					glm::vec3 p_at = this->eval_bezier(u,v);
					
					glm::vec3 p_west = this->eval_bezier(u-nabla,v);
					glm::vec3 p_north = this->eval_bezier(u,v-nabla);
					glm::vec3 p_east = this->eval_bezier(u+nabla,v);
					glm::vec3 p_south = this->eval_bezier(u,v+nabla);
					
					glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(p_east-p_west), glm::normalize(p_south-p_north)));
					
					pos_map.insert({{x,y}, p_at});
					nor_map.insert({{x,y}, normal});
				}
			}
			
			for(unsigned y=0; y+1<detail; y++)
			{
				for(unsigned x=0; x+1<detail; x++)
				{
					//Top-left 
					m_EvalMesh[pos].data << pos_map[{x  ,y  }];
					m_EvalMesh[nor].data << nor_map[{x  ,y  }];
					
					//Top-right
					m_EvalMesh[pos].data << pos_map[{x+1,y  }];
					m_EvalMesh[nor].data << nor_map[{x+1,y  }];
					
					//Bottom-left
					m_EvalMesh[pos].data << pos_map[{x  ,y+1}];
					m_EvalMesh[nor].data << nor_map[{x  ,y+1}];
					
					//
					
					//Top-right
					m_EvalMesh[pos].data << pos_map[{x+1,y  }];
					m_EvalMesh[nor].data << nor_map[{x+1,y  }];
					
					//Bottom-left
					m_EvalMesh[pos].data << pos_map[{x  ,y+1}];
					m_EvalMesh[nor].data << nor_map[{x  ,y+1}];
					
					//Bottom-right
					m_EvalMesh[pos].data << pos_map[{x+1,y+1}];
					m_EvalMesh[nor].data << nor_map[{x+1,y+1}];
				}
			}
			
			pos_map.clear();
			nor_map.clear();
			
			m_EvalMesh.upload();
			
			if(diffuse_shader != NULL)
			{
				diffuse_shader->use();
				m_EvalMesh.bind();
			}
		};
		
		void draw(glm::mat4 matView, glm::mat4 matProj)
		{
			if(wireframe_shader != NULL)
			{
				wireframe_shader->use();
				glUniformMatrix4fv(glGetUniformLocation(wireframe_shader->handle(), "uMVP"), 1, 0, glm::value_ptr(matProj * matView));
				m_WireMesh.draw();
			}
			
			if(diffuse_shader != NULL)
			{
				diffuse_shader->use();
				glUniformMatrix4fv(glGetUniformLocation(diffuse_shader->handle(), "uModelView"), 1, 0, glm::value_ptr(matView));
				glUniformMatrix4fv(glGetUniformLocation(diffuse_shader->handle(), "uProjection"), 1, 0, glm::value_ptr(matProj));
				glUniform3f(glGetUniformLocation(diffuse_shader->handle(), "uLightDir"), 0.707f,0.707f, 0.0f);
				m_EvalMesh.draw();
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
		glm::vec2	m_CameraRot;
		float		m_CameraDst;
		bool		m_CameraGrabbed;
		glm::vec2	m_CameraGrabAt;
		
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
			glClearDepth(1.0);
			glEnable(GL_DEPTH_TEST);
			
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
			
			m_DiffuseShader.link();
			m_WireShader.link();
			glBindFragDataLocation(m_DiffuseShader.handle(), 0, "outColor");
			glBindFragDataLocation(m_WireShader.handle(), 0, "outColor");
			
			std::cout << "Ready to use" << std::endl;
			
			m_Poly.wireframe_shader = &m_WireShader;
			m_Poly.diffuse_shader = &m_DiffuseShader;
			m_Poly.resize(4,4, 2.0, 8.0);
			
			m_Poly.build(32);
			
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
			
			glMatrixMode(GL_PROJECTION);
			m_Projection = glm::perspective(glm::radians(60.0f), (float)m_Aspect, 1.0f/64.0f, 64.0f);
			glLoadMatrixf(glm::value_ptr(m_Projection));
		}
		
		void on_mousebutton(int button, int action, int mods)
		{
			if(button == GLFW_MOUSE_BUTTON_RIGHT)
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
		}
		
		void on_mousepos(double x, double y)
		{
			m_Mouse = glm::vec2(x,y);
			if(m_CameraGrabbed)
			{
				glm::vec2 delta = m_CameraGrabAt - m_Mouse;
				m_CameraRot.x -= delta.y / 64.0f;
				m_CameraRot.y -= delta.x / 64.0f;
				
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
			
			//Draw
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			m_CameraAt = dirvec(m_CameraRot.y, m_CameraRot.x) * m_CameraDst;
			m_View = glm::lookAt(m_CameraAt, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f));
			
			m_Poly.draw(m_View,m_Projection);
			
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
		return 3;
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}