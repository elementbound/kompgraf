#include "window.h"

//===========================================================================================//
//Static callbacks

std::map<GLFWwindow*,window*> window::s_Window;

void window::cb_unknown_window(GLFWwindow* wnd)
{
	//TODO: Throw an exception or smth?
}

void window::cb_reposition(GLFWwindow* wnd, int x, int y)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_reposition(x, y);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_resize(GLFWwindow* wnd, int w, int h)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_resize(w, h);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_fbresize(GLFWwindow* wnd, int w, int h)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_fbresize(w, h);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_refresh(GLFWwindow* wnd)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_refresh();
	else 
		cb_unknown_window(wnd); 
}

void window::cb_iconify(GLFWwindow* wnd, int iconified)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_minimize(iconified == GL_TRUE);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_focus(GLFWwindow* wnd, int focused)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_focus(focused == GL_TRUE);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_close(GLFWwindow* wnd)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_close();
	else 
		cb_unknown_window(wnd); 
}

void window::cb_mousebutton(GLFWwindow* wnd, int button, int action, int mods)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_mousebutton(button, action, mods);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_mousepos(GLFWwindow* wnd, double x, double y)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_mousepos(x, y);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_mousefocus(GLFWwindow* wnd, int state)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_mousefocus(state == GL_TRUE);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_mousescroll(GLFWwindow* wnd, double x, double y)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_mousescroll(x, y);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_key(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_key(key, scancode, action, mods);
	else 
		cb_unknown_window(wnd); 
}

void window::cb_char(GLFWwindow* wnd, unsigned int code)
{
	auto it = s_Window.find(wnd);
	if(it != s_Window.end())
		it->second->on_char(code);
	else 
		cb_unknown_window(wnd); 
}

//===========================================================================================//
//Redefinable callbacks

void window::on_reposition(int x, int y){}
void window::on_resize(int w, int h){}
void window::on_fbresize(int w, int h){}
void window::on_refresh(){}
void window::on_focus(bool focused){}
void window::on_minimize(bool minimized){}

void window::on_open(){}
void window::on_close(){}

void window::on_mousebutton(int button, int action, int mods){}
void window::on_mousepos(double x, double y){}
void window::on_mousefocus(bool focused){}
void window::on_mousescroll(double x, double y){}
void window::on_key(int key, int scancode, int action, int mods){}
void window::on_char(unsigned int code){}

//===========================================================================================//
//Actual window guts

window::window()
{
	m_Window = NULL;
}

window::~window()
{
	close();
}

bool window::open(int w, int h, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
{
	if(this->is_valid())
		return 0;
	
	m_Window = glfwCreateWindow(w,h, title, monitor, share);
	if(m_Window == NULL)
		return 0;
		
	s_Window.insert({m_Window, this});
	glfwSetWindowPosCallback(m_Window, window::cb_reposition);
	glfwSetWindowRefreshCallback(m_Window, window::cb_refresh);
	glfwSetWindowSizeCallback(m_Window, window::cb_resize);
	glfwSetWindowCloseCallback(m_Window, window::cb_close);
	glfwSetWindowFocusCallback(m_Window, window::cb_focus);
	glfwSetWindowIconifyCallback(m_Window, window::cb_iconify);
	glfwSetFramebufferSizeCallback(m_Window, window::cb_fbresize);
	
	glfwSetMouseButtonCallback(m_Window, cb_mousebutton);
	glfwSetScrollCallback(m_Window, cb_mousescroll);
	glfwSetCursorPosCallback(m_Window, cb_mousepos);
	glfwSetKeyCallback(m_Window, cb_key);
	glfwSetCharCallback(m_Window, cb_char);
	glfwSetCursorEnterCallback(m_Window, cb_mousefocus);
	
	this->on_open();
	
	return true;
}

bool window::make_current()
{
	if(!is_valid())
		return 0;
		
	glfwMakeContextCurrent(m_Window);
	return 1;
}

bool window::close()
{
	if(!is_valid())
		return 0;
		
	glfwDestroyWindow(m_Window);
	s_Window.erase(m_Window);
	m_Window = NULL;
	return 1;
}

const GLFWwindow* window::handle() const {return m_Window;}
GLFWwindow* window::handle() {return m_Window;}

const GLFWwindow* window::operator()() const {return m_Window;}
GLFWwindow* window::operator()() {return m_Window;}

bool window::is_valid() const {return m_Window != NULL;}
window::operator bool() const {return this->is_valid();}

void window::refresh(){this->on_refresh();}