#define GLM_SWIZZLE
#include "subdiv_window.h"

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm> //std::min, std::swap
#include <random>

#include <cstdlib> //std::exit
#include <cmath>

#include "glwrap/util.h"

#define die(msg) {std::cerr << msg << std::endl; std::exit(1);}
#define dieret(msg, val) {std::cerr << msg << std::endl; return val;}

//Not strictly for existence, more like for accessibility
//Which I basically need this for, so move on... 
bool file_exists(const char* fname)
{
	std::ifstream fs(fname);
	return fs.good();
}

bool subdiv_window::init_glew()
{
	int err;
	if((err=glewInit()) != GLEW_OK)
		dieret("GLEW init: " << glewGetErrorString(err), 0);

	if(!GLEW_VERSION_3_3)
		dieret("OpenGL 3.3 not supported", 0);
	
	return 1;
}

void subdiv_window::init_window()
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

bool subdiv_window::load_resources()
{
	std::vector<const char*> file_list = 
	{
		"data/wire.vs",
		"data/wire.fs"
	};
	
	bool fail = false;
	for(const char* fname : file_list) 
		if(!file_exists(fname))
		{
			std::cerr << '\"' << fname << "\" missing!\n";
			fail = true;
		}
		
	if(fail) 
		return 0;
	
	std::cout << "Compiling wire shader... ";
		m_WireShader.create();
		
		if(!m_WireShader.attach(read_file("data/wire.vs").c_str(), shader_program::shader_type::vertex))
			dieret("\nCouldn't attach vertex shader", 0);
		
		if(!m_WireShader.attach(read_file("data/wire.fs").c_str(), shader_program::shader_type::fragment))
			dieret("\nCouldn't attach fragment shader", 0);
		
		glBindFragDataLocation(m_WireShader.handle(), 0, "outColor");
		m_WireShader.link();
	std::cout << "done\n";
	
	return 1;
}

void subdiv_window::rebuild()
{
	m_BasePoly.build_meshes();
	
	//Init with base data
	m_SubdivPoly.data() = m_BasePoly.data();
	
	//Iteratively subdivide
	std::mt19937 rd;
	std::uniform_real_distribution<float> rng(0.0f, 1.0f);
	rd.seed(0);
	
	polygon newPoly;
	for(unsigned i = 0; i<m_SubdivIterations && !m_SubdivPoly.data().empty(); i++)
	{
		for(unsigned j = 0; j < m_SubdivPoly.size(); j++)
		{
			unsigned max_index = m_SubdivPoly.size()-1;
			unsigned indices[3] = {j, 
								   (j+1)%(max_index+1), 
								   (j+2)%(max_index+1)};
								   
			glm::vec2 vertices[3] = {m_SubdivPoly[indices[0]], 
									 m_SubdivPoly[indices[1]], 
									 m_SubdivPoly[indices[2]]};
							
			float mid_f = rng(rd);
			float off_f = 0.125f + rng(rd) * 0.125f;
			
			glm::vec2 mid = mid_f*vertices[0] + (1.0f-mid_f)*vertices[1];
			glm::vec2 off = (vertices[1] - vertices[2])*off_f;
									 
			newPoly.add(vertices[0]);
			newPoly.add(mid + off);
		}
		
		std::swap(m_SubdivPoly.data(), newPoly.data());
		newPoly.clear();
	}
	
	//Build mesh
	m_SubdivMesh.clear_streams();
	m_SubdivMesh.draw_mode = GL_LINE_LOOP;
	m_SubdivMesh.storage_policy = GL_DYNAMIC_DRAW;
	unsigned pos = m_SubdivMesh.add_stream();
	m_SubdivMesh[pos].type = GL_FLOAT;
	m_SubdivMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_SubdivMesh[pos].components = 2;
	m_SubdivMesh[pos].normalized = 0;
	m_SubdivMesh[pos].name = "vertexPosition";
	
	for(unsigned i=0; i<m_SubdivPoly.size(); i++)
		m_SubdivMesh[pos].data << m_SubdivPoly[i];
	
	m_SubdivMesh.upload();
	
	//Bind meshes
	m_WireShader.use();
	m_BasePoly.control_mesh().bind();
	m_BasePoly.knot_mesh().bind();
	m_SubdivMesh.bind();
}

void subdiv_window::on_open()
{
	this->make_current();
	
	if(!init_glew())
		die("GLEW init fail!");
	
	init_window();
	
	if(!load_resources())
		die("Couldn't load resources!");
}

void subdiv_window::on_fbresize(int w, int h)
{
	resizable_window::on_fbresize(w,h);
	
	w += (w==0);
	h += (h==0);
	
	m_Ortho = glm::ortho(0.0f,(float)w, 0.0f,(float)h, -1.0f, 1.0f);
	m_Viewport = glm::vec4(0.0f, 0.0f, (float)m_FramebufferWidth, (float)m_FramebufferHeight);
}

void subdiv_window::on_mousepos(double x, double y)
{
	m_Mouse = glm::vec2(x, m_FramebufferHeight - y);
	
	if(m_Editing) 
	{
		m_BasePoly.edit(m_Mouse, m_View, m_Ortho, m_Viewport);
		rebuild();
	}
}

void subdiv_window::on_key(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(this->handle(), 1);
	
	if(key == GLFW_KEY_Q && !m_Editing && action == GLFW_PRESS)
	{
		glm::vec2 point = m_Mouse; //Screen-space
		point = glm::unProject(glm::vec3(m_Mouse.x, m_Mouse.y, 0.0f), m_View, m_Ortho, m_Viewport).xy(); //World-space 
		m_BasePoly.add(point);
	}
	
	if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
	{
		m_SubdivIterations++;
		std::cout << "Subdivs: " << m_SubdivIterations << std::endl;
	}
	
	if(key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
	{
		if(m_SubdivIterations > 0)
			m_SubdivIterations--;
		
		std::cout << "Subdivs: " << m_SubdivIterations << std::endl;
	}
	
	rebuild();
}

void subdiv_window::on_mousebutton(int button, int action, int mods)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if(action == GLFW_PRESS)
		{
			m_GrabId = m_BasePoly.grab(m_Mouse, m_View, m_Ortho, m_Viewport);
			m_Editing = (m_GrabId >= 0);
			if(m_Editing) glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if(action == GLFW_RELEASE)
		{
			m_Editing = 0;
			m_BasePoly.ungrab();
			glfwSetInputMode(this->handle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

void subdiv_window::on_refresh()
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	m_WireShader.use();
	m_WireShader.set_uniform("uMVP", m_Ortho * m_View);
	
	m_WireShader.set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
	//m_BasePoly.control_mesh().draw();
	m_BasePoly.knot_mesh().draw();
	
	m_WireShader.set_uniform("uColor", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	m_SubdivMesh.draw();
	
	glfwSwapBuffers(this->handle());
}