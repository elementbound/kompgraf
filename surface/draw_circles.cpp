#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "glwrap/util.h" //dirvec
#include "draw_circles.h"

void drawCircles::begin(bool outline)
{
	m_Mesh.clear_streams();
	m_Mesh.storage_policy = GL_STREAM_DRAW;
	m_Mesh.draw_mode = GL_LINES;
	
	pos = m_Mesh.add_stream();
	m_Mesh[pos].type = GL_FLOAT;
	m_Mesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_Mesh[pos].components = 2;
	m_Mesh[pos].normalized = 0;
	m_Mesh[pos].name = "vertexPosition";
}

void drawCircles::add_circle(float x, float y, float r, unsigned detail)
{
	for(unsigned i=0; i<detail; i++)
	{
		m_Mesh[pos].data << (glm::vec2(x,y) + r * dirvec(glm::two_pi<float>() * (i/float(detail))));
		m_Mesh[pos].data << (glm::vec2(x,y) + r * dirvec(glm::two_pi<float>() * ((i+1)/float(detail))));
	}
}

void drawCircles::end()
{
	m_Mesh.upload();
	m_Mesh.bind();
	m_Mesh.draw();
}

void drawCircles::single_circle(float x, float y, float r, bool outline, unsigned detail)
{
	begin(outline);
	add_circle(x,y,r, detail);
	end();
}

separated_mesh 	drawCircles::m_Mesh;
unsigned 		drawCircles::pos;