#include "separated_mesh.h"

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