#include "subdiv_window.h"

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <fstream>

#include <cstdlib> //std::exit

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
	if(glewInit() != GLEW_OK)
		dieret("GLEW init fail", 0);

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
}

void subdiv_window::on_open()
{
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