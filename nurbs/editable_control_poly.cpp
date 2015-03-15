#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "editable_control_poly.h"
#include "frame/util.h" // buffer << glm::vec2

int editable_control_poly::grab(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matOrtho, glm::vec4 viewport)
{
	m_GrabIndex = -1;
	
	for(unsigned i=0; i<m_Data.size(); i++)
	{
		glm::vec3 raw = glm::vec3(m_Data[i].x, m_Data[i].y, 0.0f);
		glm::vec3 projected = glm::project(raw, matView, matOrtho, viewport);
		
		if(glm::distance(projected.xy(), mouse) < grab_radius)
		{
			m_GrabIndex = i;
			break;
		}
	}
	
	return m_GrabIndex;
}

void editable_control_poly::edit(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matOrtho, glm::vec4 viewport)
{
	if(m_GrabIndex < 0)
		return; 
	
	
	m_Data[m_GrabIndex] = glm::unProject(glm::vec3(mouse.x, mouse.y, 0.0f), matView, matOrtho, viewport).xy();
}

void editable_control_poly::ungrab()
{
	m_GrabIndex = -1;
}

void editable_control_poly::build_mesh()
{
	m_Mesh.clear_streams();
	m_Mesh.storage_policy = GL_DYNAMIC_DRAW;
	m_Mesh.draw_mode = GL_LINES;
	
	unsigned pos = m_Mesh.add_stream();
	m_Mesh[pos].type = GL_FLOAT;
	m_Mesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_Mesh[pos].components = 2;
	m_Mesh[pos].normalized = 0;
	m_Mesh[pos].name = "vertexPosition";
	
	for(unsigned i=1; i<m_Data.size(); i++)
		m_Mesh[pos].data << m_Data[i-1] << m_Data[i]; 
	
	m_Mesh.upload();
}

separated_mesh& editable_control_poly::mesh()
{
	return m_Mesh;
}