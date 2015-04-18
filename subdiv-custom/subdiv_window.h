#ifndef _H_SUBDIV_WINDOW_H_
#define _H_SUBDIV_WINDOW_H_

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "glwrap/resizable_window.h"
#include "glwrap/shader.h"
#include "glwrap/mesh.h"

#include "polygon.h"
#include "editable_polygon.h"

class subdiv_window : public resizable_window
{
	private: 
		shader_program			m_WireShader;
		editable_polygon		m_BasePoly;
		polygon					m_SubdivPoly;
		
		glm::mat4	m_View;
		glm::mat4	m_Ortho;
		glm::vec2	m_Mouse;
		glm::vec4 	m_Viewport;
		
		bool m_Editing = 0;
		unsigned m_FullQuality = 16;
		unsigned m_EditQuality = 64;
		int m_GrabId = -1;
		
		bool init_glew();
		void init_window();
		bool load_resources();
		
		void rebuild();
		
	protected: 
		void on_open();
		void on_fbresize(int w, int h);
		void on_mousepos(double x, double y);
		void on_key(int key, int scancode, int action, int mods);
		void on_mousebutton(int button, int action, int mods);
		void on_refresh();
};

#endif