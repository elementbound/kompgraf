#include "separated_mesh.h"
#include <iostream>

inline unsigned gl_type_size(GLenum type)
{
	//GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT
	//GL_HALF_FLOAT, GL_FLOAT, GL_DOUBLE, GL_FIXED, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV
	switch(type)
	{
		case GL_BYTE: 
		case GL_UNSIGNED_BYTE: 
			return 1; 
		break;
		
		case GL_SHORT: 
		case GL_UNSIGNED_SHORT: 
		case GL_HALF_FLOAT:
			return 2;
		break;
		
		case GL_INT: 
		case GL_UNSIGNED_INT: 
		case GL_FLOAT: 
		
		case GL_INT_2_10_10_10_REV:
		case GL_UNSIGNED_INT_10F_11F_11F_REV: 
		case GL_UNSIGNED_INT_2_10_10_10_REV: 
			return 4;
		break;
		
		case GL_DOUBLE: 
			return 8;
		break;
		
		default: 
			return 0;
	}
}

separated_mesh::~separated_mesh()
{
	for(auto& p : m_VBOs)
		glDeleteBuffers(1, &p.second);
	
	glDeleteVertexArrays(1, &m_VAO);
}

void separated_mesh::upload()
{
	for(std::pair<const unsigned, stream_data>& p: m_Streams)
	{
		if(!m_VBOs.count(p.first))
		{
			GLuint id;
			glGenBuffers(1, &id);
			m_VBOs.insert({p.first, id});
		}
		
		glBindBuffer(p.second.buffer_type, m_VBOs[p.first]);
		glBufferData(p.second.buffer_type, p.second.data.size(), p.second.data.data(), GL_STATIC_DRAW);
	}
}

void separated_mesh::bind()
{
	if(m_VAO == 0)
	{
		glGenVertexArrays(1, &m_VAO);
		std::cout << "Acquired VAO#" << m_VAO << std::endl;
	}
	
	glBindVertexArray(m_VAO);
	for(auto& p : m_Streams)
	{
		/*
		struct stream_data
		{
			unsigned index; 
			GLenum type;
			GLenum buffer_type;
			unsigned components;
			bool normalized;
			
			buffer data;
		};
		
		void glVertexAttribPointer( 	
			GLuint index,
			GLint size,
			GLenum type,
			GLboolean normalized,
			GLsizei stride,
			const GLvoid * pointer);
		*/
		glBindBuffer(p.second.buffer_type, m_VBOs[p.first]);
		glEnableVertexAttribArray(p.second.index);
		glVertexAttribPointer(p.second.index, p.second.components, p.second.type, p.second.normalized, 0, 0);
	}
}

void separated_mesh::draw()
{
	if(m_VBOs.empty())
		return;
	
	unsigned vertex_count = 0;
	const stream_data& sd = m_Streams.begin()->second;
	vertex_count  = sd.data.size() / (sd.components * gl_type_size(sd.type));
	
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertex_count);
}