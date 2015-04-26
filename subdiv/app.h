#ifndef _H_SUBDIV_APP_H_
#define _H_SUBDIV_APP_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "glwrap/resizable_window.h"
#include "glwrap/shader.h"
#include "glwrap/mesh.h"

#include "model.h"

#include <stack>
#include <string>

class app_Subdiv : public resizable_window
{
	private: 
		shader_program			m_DiffuseShader;
		model					m_Model;
		separated_mesh			m_Mesh;
		
		glm::mat4	m_View;
		glm::mat4	m_Projection;
		glm::vec4	m_Viewport;
		glm::vec2	m_Mouse;

		glm::vec2	m_CameraRot = glm::vec2(glm::radians(30.0f), glm::radians(45.0f));
		float		m_CameraDst = 4.0f;
		glm::vec3	m_CameraAt;
		float		m_CameraFov = glm::radians(60.0f);
		bool		m_CameraGrabbed = 0;
		glm::vec2	m_CameraGrabAt;

		std::stack<model> m_SubdivStack;
		
		bool init_glew();
		void init_window();
		bool load_resources();

		std::string get_open_filename();
		
		void rebuild();
		
	protected: 
		void on_open();
		void on_fbresize(int w, int h);
		void on_mousepos(double x, double y);
		void on_mousebutton(int button, int action, int mods);
		void on_mousescroll(double x, double y);
		void on_key(int key, int scancode, int action, int mods);
		void on_refresh();
};

#endif