#include "control_poly.h"
#include "frame/util.h"

unsigned control_surface::hash(unsigned row, unsigned column) const {
	return column + row*m_Columns;
}

control_surface::control_surface(const control_surface& rhs)
{
	m_Points = rhs.m_Points;
	m_Rows = rhs.m_Rows;
	m_Columns = rhs.m_Columns;
}

control_surface& control_surface::operator=(const control_surface& rhs)
{
	m_Points = rhs.m_Points;
	m_Rows = rhs.m_Rows;
	m_Columns = rhs.m_Columns;
}

glm::vec3&			control_surface::operator()(unsigned row, unsigned column) 		 {return this->get(row, column);}
const glm::vec3&	control_surface::operator()(unsigned row, unsigned column) const {return this->get(row, column);}
glm::vec3&			control_surface::get(unsigned row, unsigned column)			{return m_Points[hash(row, column)];}
const glm::vec3&	control_surface::get(unsigned row, unsigned column) const	{return m_Points[hash(row, column)];}

unsigned control_surface::rows() const {return this->m_Rows;}
unsigned control_surface::columns() const {return this->m_Columns;}

void control_surface::resize(unsigned rows, unsigned columns)
{
	m_Rows = rows;
	m_Columns = columns;
	m_Points.resize(m_Rows * m_Columns);
}

void control_surface::build_mesh()
{
	m_Mesh.clear_streams();
	m_Mesh.storage_policy = GL_DYNAMIC_DRAW;
	m_Mesh.draw_mode = GL_LINES;
	
	unsigned pos = m_Mesh.add_stream();
	m_Mesh[pos].type = GL_FLOAT;
	m_Mesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_Mesh[pos].components = 3;
	m_Mesh[pos].normalized = 0;
	m_Mesh[pos].name = "vertexPosition";
	
	for(unsigned row=0; row<m_Rows; row++)
	{
		for(unsigned col=0; col<m_Columns; col++)
		{
			if(col+1 < m_Columns)
			{
				m_Mesh[pos].data << m_Points[hash(row  , col  )];
				m_Mesh[pos].data << m_Points[hash(row  , col+1)];
			}
			
			if(row+1 < m_Rows)
			{
				m_Mesh[pos].data << m_Points[hash(row  , col  )];
				m_Mesh[pos].data << m_Points[hash(row+1, col  )];
			}
		}
	}
	
	m_Mesh.upload();
}

void control_surface::draw()
{
	m_Mesh.draw();
}

separated_mesh& control_surface::mesh()
{
	return m_Mesh;
}