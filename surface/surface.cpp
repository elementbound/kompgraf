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
		
		//TODO: Get an enum or smth
		//0 - Bezier; 1 - B-Spline
		unsigned 				m_Mode = 0; 
		
		separated_mesh			m_EvalMesh;
		separated_mesh			m_ControlMesh;
		separated_mesh			m_WireMesh;
		
		int 	m_GrabIndex = -1;
		float 	m_GrabDepth;
		
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
					z = glm::perlin(glm::vec2(u,v)) * height;//std::pow(1.0f-std::abs(u), 2) * std::pow(1.0f-std::abs(v), 2) * height;
					
					m_PointMatrix[hash(x,y)] = glm::vec3(u*size, v*size, z);
				}
			}
		}
		
		int grab(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matPerspective)
		{
			m_GrabIndex = -1;
			
			int viewport[4]; //[x,y,w,h]
			glGetIntegerv(GL_VIEWPORT, viewport);
			
			for(unsigned i=0; i<m_PointMatrix.size(); i++)
			{
				glm::vec3& p = m_PointMatrix[i];
				glm::vec3 projected = glm::project(p, matView, matPerspective, glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]));
				
				if(glm::distance(projected.xy(), mouse) < 4)
				{
					m_GrabIndex = i;
					m_GrabDepth = projected.z;
					break;
				}
			}
			
			return m_GrabIndex;
		}
		
		void edit(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matPerspective)
		{
			if(m_GrabIndex < 0)
				return;
			
			int viewport[4]; //[x,y,w,h]
			glGetIntegerv(GL_VIEWPORT, viewport);
			
			glm::vec3 new_pos = glm::unProject(glm::vec3(mouse.x,mouse.y, m_GrabDepth), matView, matPerspective, glm::vec4(viewport[0],viewport[1],viewport[2],viewport[3]));
			m_PointMatrix[m_GrabIndex] = new_pos;
		}
		
		void ungrab()
		{
			m_GrabIndex = -1;
		}
		
		void build_control_mesh()
		{
			m_ControlMesh.clear_streams();
			m_ControlMesh.storage_policy = GL_DYNAMIC_DRAW;
			m_ControlMesh.draw_mode = GL_LINES;
			
			unsigned pos = m_ControlMesh.add_stream();
			m_ControlMesh[pos].type = GL_FLOAT;
			m_ControlMesh[pos].buffer_type = GL_ARRAY_BUFFER;
			m_ControlMesh[pos].components = 3;
			m_ControlMesh[pos].normalized = 0;
			m_ControlMesh[pos].name = "vertexPosition";
			
			for(unsigned y=0; y<m_PointRows; y++)
			{
				for(unsigned x=0; x<m_PointColumns; x++)
				{
					if(x+1 < m_PointColumns)
					{
						m_ControlMesh[pos].data << m_PointMatrix[hash(x  , y)];
						m_ControlMesh[pos].data << m_PointMatrix[hash(x+1, y)];
					}
					
					if(y+1 < m_PointRows)
					{
						m_ControlMesh[pos].data << m_PointMatrix[hash(x, y  )];
						m_ControlMesh[pos].data << m_PointMatrix[hash(x, y+1)];
					}
				}
			}
			
			m_ControlMesh.upload();
			
			if(wireframe_shader != NULL)
			{
				wireframe_shader->use();
				m_ControlMesh.bind();
			}
		}
		
		glm::vec3 eval(float u, float v)
		{
			if(m_Mode == 0)
				return eval_bezier(u,v);
			else if(m_Mode == 1)
				return eval_bspline(u,v);
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
		
		float weight_bspline(float t, unsigned point, unsigned point_count, unsigned order)
		{
			auto ui = [point_count](unsigned i) -> float {return i/(float)point_count;};
			
			if(order == 0)
				return (ui(point) < t && t < ui(point+1)) ? 1.0f : 0.0f;
				
			return (t-ui(point))/(ui(point+order) - ui(point)) * weight_bspline(t, point, point_count, order-1) +
				   (ui(point+order+1) - t)/(ui(point+order+1) - ui(point+1)) * weight_bspline(t, point+1, point_count, order-1);
		}
		
		glm::vec3 eval_bspline(float u, float v)
		{
			glm::vec3 p = glm::vec3(0.0f);
			
			for(unsigned row=0; row<m_PointRows; row++)
				for(unsigned col=0; col<m_PointColumns; col++)
					p += weight_bspline(u, col, m_PointColumns, m_PointColumns) * 
						 weight_bspline(v, row,	m_PointRows,    m_PointRows) * m_PointMatrix[hash(col, row)];
			
			return p;
		}
		
		void build_eval_mesh(unsigned detail)
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
								 
					glm::vec3 p_at = this->eval(u,v);
					
					glm::vec3 p_west = this->eval(u-nabla,v);
					glm::vec3 p_north = this->eval(u,v-nabla);
					glm::vec3 p_east = this->eval(u+nabla,v);
					glm::vec3 p_south = this->eval(u,v+nabla);
					
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
		}
		
		void build_wire_mesh(unsigned detail, unsigned wire_count)
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
			
			//Rows
			for(unsigned row = 0; row < wire_count; row++)
			{
				for(unsigned x=0; x+1<=detail; x++)
				{
					float u[2] = {x/(float)detail, (x+1)/(float)detail};
					float v = row/(float)(wire_count-1);
					m_WireMesh[pos].data << this->eval(u[0], v);
					m_WireMesh[pos].data << this->eval(u[1], v);
				}
			}
			
			//Columns
			for(unsigned column = 0; column < wire_count; column++)
			{
				for(unsigned y=0; y+1<=detail; y++)
				{
					float u = column/(float)(wire_count-1);
					float v[2] = {y/(float)detail, (y+1)/(float)detail};
					m_WireMesh[pos].data << this->eval(u, v[0]);
					m_WireMesh[pos].data << this->eval(u, v[1]);
				}
			}
			
			//
			
			m_WireMesh.upload();
			if(wireframe_shader != NULL)
			{
				wireframe_shader->use();
				m_WireMesh.bind();
			}
		}
		
		void build(unsigned detail, unsigned wire_count)
		{
			build_control_mesh();
			build_eval_mesh(detail);
			build_wire_mesh(detail, wire_count);
		}
		
		void draw(glm::mat4 matView, glm::mat4 matPerspective, glm::mat4 matOrtho)
		{
			if(diffuse_shader != NULL)
			{
				glEnable(GL_DEPTH_TEST);
				
				diffuse_shader->use();
				diffuse_shader->set_uniform("uModelView", matView); 
				diffuse_shader->set_uniform("uProjection", matPerspective); 
				diffuse_shader->set_uniform("uLightDir", glm::vec3(0.707f, 0.707f, 0.0f)); 
				m_EvalMesh.draw();
			}
			
			if(wireframe_shader != NULL)
			{
				wireframe_shader->use();
				wireframe_shader->set_uniform("uMVP", matPerspective * matView); 
				
				glDisable(GL_DEPTH_TEST);
				wireframe_shader->set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.25f));
				m_ControlMesh.draw();
				m_WireMesh.draw();
				
				glEnable(GL_DEPTH_TEST);
				wireframe_shader->set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
				m_ControlMesh.draw();
				m_WireMesh.draw();
				
				glDisable(GL_DEPTH_TEST);
				
				int viewport[4]; //[x,y,w,h]
				glGetIntegerv(GL_VIEWPORT, viewport);
				
				wireframe_shader->set_uniform("uMVP", matOrtho);
				wireframe_shader->set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
				
				drawCircles::begin(1);
				for(glm::vec3& p: m_PointMatrix)
				{
					glm::vec3 projected = glm::project(p, matView, matPerspective, glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]));
					drawCircles::add_circle(projected.x, projected.y, 4);//glcCircle(projected.x, projected.y, 4, 1);
				}
				drawCircles::end();
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
		glm::mat4	m_Perspective;
		glm::mat4	m_Ortho;
		glm::vec2	m_Mouse;
		
		glm::vec3	m_CameraAt;
		glm::vec2	m_CameraRot;
		float		m_CameraDst;
		bool		m_CameraGrabbed = 0;
		glm::vec2	m_CameraGrabAt;
		
		int m_Width, m_Height;
		
		editable_poly	m_Poly;
		bool 			m_Editing = 0;
		unsigned		m_FullQuality = 32;
		unsigned		m_EditQuality = 8;
		unsigned		m_GridDensity = 8;
		
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
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
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
			m_Poly.resize(4,4, 2.0, 2.0);
			
			m_Poly.build(m_FullQuality, m_GridDensity);
			
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
					int grab = m_Poly.grab(m_Mouse, m_View, m_Perspective);
					if(grab >= 0)
					{
						glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						m_Editing = 1;
					}
				}
				else if(action == GLFW_RELEASE)
				{
					if(m_Editing)
						m_Poly.build(m_FullQuality, m_GridDensity);
					
					m_Poly.ungrab();
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
				m_Poly.edit(m_Mouse, m_View, m_Perspective);
				m_Poly.build(m_EditQuality, m_GridDensity);
			}
			
			//Draw
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			m_CameraAt = dirvec(m_CameraRot.y, m_CameraRot.x) * m_CameraDst;
			m_View = glm::lookAt(m_CameraAt, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f));
			
			m_Poly.draw(m_View, m_Perspective, m_Ortho);
			
			m_WireShader.use();
			m_WireShader.set_uniform("uMVP", m_Ortho);
			//glcCircle(m_Mouse.x, m_Mouse.y, 4, 1);
			
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
	
	glfwSwapInterval(0);
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}