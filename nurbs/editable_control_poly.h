#ifndef _H_EDITABLE_CONTROL_POLY_H_ 
#define _H_EDITABLE_CONTROL_POLY_H_

#include <glm/glm.hpp>
#include "glwrap/mesh.h"
#include "control_poly.h"

class editable_control_poly : public control_poly 
{
	private: 
		int m_GrabIndex;
		separated_mesh m_ControlMesh; 
		separated_mesh m_KnotsMesh;
		
	public: 
		unsigned grab_radius = 4;
		unsigned knot_detail = 16;
		
		int grab(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matOrtho, glm::vec4 viewport);
		void edit(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matOrtho, glm::vec4 viewport);
		void ungrab();
		
		void build_meshes();
		separated_mesh& control_mesh();
		separated_mesh& knot_mesh();
};

#endif