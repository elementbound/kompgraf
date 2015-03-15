#define GLM_SWIZZLE
#include <GLM/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "editable_control_surface.h"
#include "draw_circles.h"

int 	editable_control_surface::grab(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matPerspective, glm::vec4 viewport)
{
	m_GrabIndex = -1;

	for(unsigned i=0; i<m_Points.size(); i++)
	{
		glm::vec3& p = m_Points[i];
		glm::vec3 projected = glm::project(p, matView, matPerspective, viewport);
		
		if(glm::distance(projected.xy(), mouse) < grab_radius)
		{
			m_GrabIndex = i;
			m_GrabDepth = projected.z;
			break;
		}
	}

	return m_GrabIndex;
}

void 	editable_control_surface::edit(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matPerspective, glm::vec4 viewport)
{
	if(m_GrabIndex < 0)
		return;
	
	m_Points[m_GrabIndex] = glm::unProject(glm::vec3(mouse.x,mouse.y, m_GrabDepth), matView, matPerspective, viewport);
}

void 	editable_control_surface::ungrab()
{
	m_GrabIndex = -1;
}

void	editable_control_surface::draw_grabs(glm::mat4 matView, glm::mat4 matPerspective, glm::vec4 viewport)
{
	drawCircles::begin(1);
	for(glm::vec3& p: m_Points)
	{
		glm::vec3 projected = glm::project(p, matView, matPerspective, viewport);
		drawCircles::add_circle(projected.x, projected.y, grab_radius);
	}
	drawCircles::end();
	
	if(m_GrabIndex > 0)
	{
		glm::vec3 projected = glm::project(m_Points[m_GrabIndex], matView, matPerspective, viewport);
		drawCircles::single_circle(projected.x, projected.y, grab_radius, 0);
	}
}