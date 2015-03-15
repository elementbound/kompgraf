#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "frame/util.h"
#include "frame/resizable_window.h"
#include "frame/shader.h"
#include "frame/mesh.h"

#include <iostream> 

#define DEBUG(msg) std::cout << msg << std::endl

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
}

class control_poly 
{
	private: 
		std::vector<glm::vec2> m_Data; 
		
	public: 
		glm::vec2& 			get(unsigned i);
		const glm::vec2&	get(unsigned i) const;
		glm::vec2&			operator()(unsigned i);
		const glm::vec2&	operator()(unsigned i) const;
		
		unsigned size() const;
		void resize(unsigned s);
};

class nurbs_window : public resizable_window
{
};

int main()
{
	glfwSetErrorCallback(error_callback);
	if(!glfwInit())
		return 1;
	
	nurbs_window wnd;
	
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