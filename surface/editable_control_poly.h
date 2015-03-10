#ifndef _H_EDITABLE_CONTROL_POLY_H_
#define _H_EDITABLE_CONTROL_POLY_H_

#include "control_poly.h"

class editable_control_surface : public control_surface 
{
	private: 
		int		m_GrabIndex = -1;
		float	m_GrabDepth; 
	public: 
		int 	grab(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matPerspective, glm::vec4 viewport);
		void 	edit(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matPerspective, glm::vec4 viewport);
		void 	ungrab();
		
		void	draw_grabs(glm::mat4 matView, glm::mat4 matPerspective, glm::vec4 viewport);
		
		unsigned grab_radius = 4;
};

#endif