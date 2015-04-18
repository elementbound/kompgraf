#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>

#include "editable_polygon.h"
#include "glwrap/util.h" // buffer << glm::vec2

int editable_polygon::grab(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matProj, glm::vec4 viewport)
{
	m_GrabIndex = -1;
	
	for(unsigned i=0; i<m_Data.size(); i++)
	{
		glm::vec3 raw = glm::vec3(m_Data[i].x, m_Data[i].y, 0.0f);
		glm::vec3 projected = glm::project(raw, matView, matProj, viewport);
		
		if(glm::distance(projected.xy(), mouse) < grab_radius)
		{
			m_GrabIndex = i;
			break;
		}
	}
	
	return m_GrabIndex;
}

void editable_polygon::edit(glm::vec2 mouse, glm::mat4 matView, glm::mat4 matProj, glm::vec4 viewport)
{
	if(m_GrabIndex < 0)
		return; 
	
	
	m_Data[m_GrabIndex] = glm::unProject(glm::vec3(mouse.x, mouse.y, 0.0f), matView, matProj, viewport).xy();
}

void editable_polygon::ungrab()
{
	m_GrabIndex = -1;
}

void editable_polygon::build_meshes()
{
	m_ControlMesh.clear_streams();
	m_ControlMesh.storage_policy = GL_DYNAMIC_DRAW;
	m_ControlMesh.draw_mode = GL_LINES;
	
	unsigned pos = m_ControlMesh.add_stream();
	m_ControlMesh[pos].type = GL_FLOAT;
	m_ControlMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_ControlMesh[pos].components = 2;
	m_ControlMesh[pos].normalized = 0;
	m_ControlMesh[pos].name = "vertexPosition";
	
	for(unsigned i=1; i<m_Data.size(); i++)
		m_ControlMesh[pos].data << m_Data[i-1] << m_Data[i]; 
	
	m_ControlMesh.upload();
	
	//
	
	m_KnotsMesh.clear_streams();
	m_KnotsMesh.storage_policy = GL_DYNAMIC_DRAW;
	m_KnotsMesh.draw_mode = GL_LINES;
	
	pos = m_KnotsMesh.add_stream();
	m_KnotsMesh[pos].type = GL_FLOAT;
	m_KnotsMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_KnotsMesh[pos].components = 2;
	m_KnotsMesh[pos].normalized = 0;
	m_KnotsMesh[pos].name = "vertexPosition";
	
	for(glm::vec2& p : m_Data)
	{
		for(unsigned i=0; i<knot_detail; i++)
		{
			m_KnotsMesh[pos].data << (p + (float)grab_radius * dirvec(i/(float)knot_detail * glm::two_pi<float>()));
			m_KnotsMesh[pos].data << (p + (float)grab_radius * dirvec((i+1)/(float)knot_detail * glm::two_pi<float>()));
		}
	}
	
	m_KnotsMesh.upload();
}

separated_mesh& editable_polygon::control_mesh()
{
	return m_ControlMesh;
}

separated_mesh& editable_polygon::knot_mesh()
{
	return m_KnotsMesh;
}