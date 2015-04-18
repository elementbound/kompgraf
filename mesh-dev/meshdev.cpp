#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glwrap/window.h"
#include "glwrap/shader.h"
#include "glwrap/mesh.h"
#include "glwrap/util.h"

#include <iostream>
#include <string>

void error_callback(int error, const char* error_str)
{
	std::cerr << "[" << error << "]" << error_str << std::endl;
}

class window_triangle: public window
{
	private:
		shader_program program;
		separated_mesh mesh;
		
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
			
			std::cout << "Creating mesh... ";
			{
				mesh.draw_mode = GL_TRIANGLE_FAN;
				mesh.storage_policy = GL_STATIC_DRAW;
				
				unsigned pos = mesh.add_stream();
				unsigned color = mesh.add_stream();
				unsigned indices = mesh.add_stream();
				
				mesh[pos].type = GL_FLOAT;
				mesh[pos].buffer_type = GL_ARRAY_BUFFER;
				mesh[pos].components = 2;
				mesh[pos].normalized = 0;
				mesh[pos].name = "vertexPosition";
				
				mesh[pos].data <<
					std::cos(glm::radians(  0.0f)) << std::sin(glm::radians(  0.0f)) << 
					std::cos(glm::radians( 90.0f)) << std::sin(glm::radians( 90.0f)) << 
					std::cos(glm::radians(180.0f)) << std::sin(glm::radians(180.0f)) << 
					std::cos(glm::radians(270.0f)) << std::sin(glm::radians(270.0f));
					
				//
				
				mesh[color].type = GL_FLOAT;
				mesh[color].buffer_type = GL_ARRAY_BUFFER;
				mesh[color].components = 3;
				mesh[color].normalized = 0;
				mesh[color].name = "vertexColor";
				
				mesh[color].data << 
					1.0f << 0.0f << 0.0f << 
					0.0f << 1.0f << 0.0f << 
					0.0f << 0.0f << 1.0f << 
					1.0f << 1.0f << 1.0f;
					
				mesh.upload();
			}
			std::cout << "Done" << std::endl;
			
			//Shaders
			std::cout << "Compiling shaders... ";
			program.create();
			
			if(!program.attach(read_file("data/simple2.vs").c_str(), shader_program::shader_type::vertex))
			{
				std::cerr << "Couldn't attach vertex shader" << std::endl;
				return;
			}
			
			if(!program.attach(read_file("data/simple2.fs").c_str(), shader_program::shader_type::fragment))
			{
				std::cerr << "Couldn't attach fragment shader" << std::endl;
				return;
			}
			
			glBindFragDataLocation(program.handle(), 0, "outColor");
			program.link();
			program.use();
			
			//Link attribs
			mesh.bind();
			
			std::cout << "Ready to use" << std::endl;
		}
		
		void on_refresh()
		{
			static float f = 0.0;
			f += 1.0/256.0;
			if(f >= 1.0) f -= 2.0f;
			
			glClear(GL_COLOR_BUFFER_BIT);
			
			float angle = fmod(glfwGetTime()/4.0, 1.0) * 2.0 * 3.14159265;
			glm::mat4 matRot;
			matRot = glm::rotate(matRot, angle, glm::vec3(0.0, 0.0, 1.0));
			int u = glGetUniformLocation(program.handle(), "uMVP");
			glUniformMatrix4fv(u, 1, 0, glm::value_ptr(matRot));
			//glDrawArrays(GL_TRIANGLES, 0, 3);
			mesh.draw();
			
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
		
	window_triangle wnd;
	
	glewExperimental = GL_TRUE;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	wnd.open(512,512, "it's a tRIANGLE");
	if(!wnd)
		return 3;
	
	wnd.make_current();
	
	std::cout << "Init done, starting loop" << std::endl;
	
	while(!glfwWindowShouldClose(wnd.handle()))
	{
		wnd.refresh();
		glfwPollEvents();
	}
	
	return 0;
}