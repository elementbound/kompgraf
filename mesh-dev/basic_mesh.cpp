#include "basic_mesh.h"
#include <iostream>

basic_mesh::basic_mesh()
{
	glGenVertexArrays(1, &m_VAO);
	std::cout << "Acquired VAO#" << m_VAO << std::endl;
}

basic_mesh::~basic_mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
	std::cout << "Released VAO#" << m_VAO << std::endl;
}

unsigned basic_mesh::add_stream()
{
	unsigned nid = 0;
	if(!m_Streams.empty())
		nid = m_Streams.rbegin()->first+1;
		
	m_Streams.insert({nid, stream_data()});
	return nid;
}

bool basic_mesh::remove_stream(unsigned id)
{
	return m_Streams.erase(id);
}

basic_mesh::stream_data& basic_mesh::get_stream(unsigned id)
{
	return m_Streams.at(id);
}

const basic_mesh::stream_data& basic_mesh::get_stream(unsigned id) const
{
	return m_Streams.at(id);
}

basic_mesh::stream_data& basic_mesh::operator[](unsigned id)
{
	return this->get_stream(id);
}

const basic_mesh::stream_data& basic_mesh::operator[](unsigned id) const 
{
	return this->get_stream(id);
}