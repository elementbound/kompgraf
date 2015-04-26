#include <iostream>
#include <glm/gtx/io.hpp>
#include <GLFW/glfw3.h>

#ifndef dbg
	#define dbg(msg) {std::cout << msg;}
	#define rtdbg(msg, interval) { \
			static double lastPrint = 0.0; \
			if(glfwGetTime() > lastPrint + interval) {\
				lastPrint = glfwGetTime(); \
				std::cout << msg << "        \r"; \
			} \
		} 
#endif