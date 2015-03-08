#include "basic_mesh.h"

basic_mesh::basic_mesh()
{
	m_VAO = 0;
	draw_mode = GL_TRIANGLES;
	storage_policy = GL_STATIC_DRAW;
}

basic_mesh::~basic_mesh()
{
	glDeleteVertexArrays(1, &m_VAO);
}

unsigned basic_mesh::add_stream()
{
	unsigned nid = 0;
	if(!m_Streams.empty())
		nid = m_Streams.rbegin()->first+1;
		
	m_Streams.insert({nid, stream_data()});
	return nid;
}

void basic_mesh::clear_streams()
{
	m_Streams.clear();
}

bool basic_mesh::remove_stream(unsigned id)
{
	return m_Streams.erase(id);
}

unsigned basic_mesh::stream_count() const
{
	return m_Streams.size();
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