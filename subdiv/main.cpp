#include "app.h"

#include <GLFW/glfw3.h>

#include <iostream> 
#include <cstdlib> //std::exit

#define die(msg) {std::cerr << msg << std::endl; std::exit(1);}

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
}

#include <set>
#include <algorithm>
#include <iterator>

int main()
{
	/*std::set<int> a = {1, 2, 3};
	std::set<int> b = {3, 4, 5};
	std::set<int> result;

	std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result));

	for(int i : result)
		std::cout << i << ' ';
	return 0;*/

	glfwSetErrorCallback(error_callback);
	if(!glfwInit())
		die("Couldn't init GLFW");
	
	app_Subdiv wnd;
	
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	
	wnd.open(640,480, "coolSUBDIV");
	if(!wnd)
		die("Couldn't create window");
	
	glfwSwapInterval(0);
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}