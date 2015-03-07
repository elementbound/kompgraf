#ifndef _H_SEPARATED_MESH_H_
#define _H_SEPARATED_MESH_H_ 

#include <map> 
#include <vector>
#include <GL/glew.h>
#include "basic_mesh.h"

class separated_mesh : public basic_mesh
{
	private: 
		std::map<unsigned, GLuint> m_VBOs;
		
	public: 
		separated_mesh() = default;
		separated_mesh(const separated_mesh&) = default;
		~separated_mesh();
	
		void upload();
		void bind();
		void draw();
};

#endif