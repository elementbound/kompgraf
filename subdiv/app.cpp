#include "app.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <vector>
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

bool app_Subdiv::init_glew()
{
	int err;
	if((err=glewInit()) != GLEW_OK)
		dieret("GLEW init: " << glewGetErrorString(err), 0);

	if(!GLEW_VERSION_3_3)
		dieret("OpenGL 3.3 not supported", 0);
	
	return 1;
}

void app_Subdiv::init_window()
{
	int w, h;
	glfwGetFramebufferSize(this->handle(), &w, &h);
	this->on_fbresize(w,h);
	
	glfwSetInputMode(this->handle(), GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	
	//GL init
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	
	m_View = glm::mat4(1.0f);
}

bool app_Subdiv::load_resources()
{
	std::vector<const char*> file_list = 
	{
		"data/diffuse.vs",
		"data/diffuse.fs"
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
	
	std::cout << "Compiling diffuse shader... ";
		m_DiffuseShader.create();
		
		if(!m_DiffuseShader.attach(read_file("data/diffuse.vs").c_str(), shader_program::shader_type::vertex))
			dieret("\nCouldn't attach vertex shader", 0);
		
		if(!m_DiffuseShader.attach(read_file("data/diffuse.fs").c_str(), shader_program::shader_type::fragment))
			dieret("\nCouldn't attach fragment shader", 0);
		
		glBindFragDataLocation(m_DiffuseShader.handle(), 0, "outColor");
		m_DiffuseShader.link();
	std::cout << "done\n";
	
	return 1;
}

void app_Subdiv::rebuild() 
{
	m_Model.build_drawable(m_Mesh);
	m_DiffuseShader.use();
	m_Mesh.bind();
}

void app_Subdiv::on_open()
{
	this->make_current();
	
	if(!init_glew())
		die("GLEW init fail!");
	
	init_window();
	
	if(!load_resources())
		die("Couldn't load resources!");

	m_Model = loadModelFromOBJ("data/smooth-cube.obj");
	rebuild();

	//m_SubdivOperator = loopSubdivOperator();
}

void app_Subdiv::on_fbresize(int w, int h)
{
	resizable_window::on_fbresize(w,h);
	
	w += (w==0);
	h += (h==0);
	
	m_Projection = glm::perspective(m_CameraFov, float(w)/h, 1e-2f, 1024.0f);
	m_Viewport = glm::vec4(0.0f, 0.0f, (float)m_FramebufferWidth, (float)m_FramebufferHeight);
}

void app_Subdiv::on_mousepos(double x, double y)
{
	m_Mouse = glm::vec2(x,m_FramebufferHeight - y);
	if(m_CameraGrabbed)
	{
		glm::vec2 delta = m_CameraGrabAt - m_Mouse;
		m_CameraRot.x = glm::clamp(m_CameraRot.x + delta.y / 64.0f, -glm::radians(89.0f), glm::radians(89.0f));
		m_CameraRot.y = std::fmod(m_CameraRot.y - delta.x / 64.0f, glm::two_pi<float>());
		
		m_CameraGrabAt = m_Mouse;
	}
}

void app_Subdiv::on_mousescroll(double x, double y)
{
	if(y > 0.0)
		m_CameraDst *= std::pow(2.0, 1.0/8.0);
	else 
		m_CameraDst /= std::pow(2.0, 1.0/8.0);
}

void app_Subdiv::on_mousebutton(int button, int action, int mods)
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

void app_Subdiv::on_key(int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE)
		glfwSetWindowShouldClose(this->handle(), 1);

	if(key == GLFW_KEY_KP_ADD && action == GLFW_PRESS)
	{
		std::cout << "Subdividing... \n";

		m_SubdivStack.push(m_Model);
		m_Model = m_SubdivOperator(m_Model);
	}
	
	if(key == GLFW_KEY_KP_SUBTRACT && action == GLFW_PRESS)
	{
		if(!m_SubdivStack.empty())
		{
			m_Model = m_SubdivStack.top();
			m_SubdivStack.pop();
		}
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		std::string fname = get_open_filename();
		if(fname != "")
		{
			m_Model = loadModelFromOBJ(fname.c_str());
			while(!m_SubdivStack.empty())
				m_SubdivStack.pop();
		}
	}
	
	rebuild();
}

void app_Subdiv::on_refresh()
{
	//Draw
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_CameraAt = dirvec(m_CameraRot.y, m_CameraRot.x) * m_CameraDst;
	m_View = glm::lookAt(m_CameraAt, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,0.0f,1.0f));

	m_DiffuseShader.use();
	m_DiffuseShader.set_uniform("uModelView", m_View); 
	m_DiffuseShader.set_uniform("uProjection", m_Projection); 
	m_DiffuseShader.set_uniform("uLightDir", glm::vec3(0.707f, 0.707f, 0.0f)); 

	m_Mesh.draw();
	
	glfwSwapBuffers(this->handle());
}

#if defined(WIN32) or defined(_WIN32) or defined(_WIN64)
	//#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	std::string app_Subdiv::get_open_filename()
	{
		//https://msdn.microsoft.com/en-us/library/ms646829%28v=vs.85%29.aspx#open_file
		//modded a bit, ofc
		OPENFILENAME ofn;       // common dialog box structure
		char szFile[260];       // buffer for file name
		HWND hwnd = NULL;       // owner window
		HANDLE hf;              // file handle

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Mesh\0*.obj\0All\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 

		if (GetOpenFileName(&ofn)==TRUE) 
			return ofn.lpstrFile;
		else
			return "";
	}
#else
	#include <iostream>

	std::string app_Subdiv::get_open_filename() 
	{
		std::string in;
		std::cin >> in;

		return in;
	}
#endif