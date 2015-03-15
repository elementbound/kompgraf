#ifndef _H_CONTROL_SURFACE_H_
#define _H_CONTROL_SURFACE_H_

#include <vector>
#include <GLM/glm.hpp>
#include "frame/mesh.h"

class control_surface
{
	protected:
		std::vector<glm::vec3> 	m_Points;
		unsigned 				m_Rows;
		unsigned 				m_Columns;
		
		separated_mesh			m_Mesh;
		
		unsigned hash(unsigned row, unsigned column) const;
		
	public: 
		control_surface() = default;;
		control_surface(const control_surface&);
		~control_surface() = default;
	
		control_surface& operator=(const control_surface& rhs);
	
		glm::vec3&			operator()(unsigned row, unsigned column);
		const glm::vec3&	operator()(unsigned row, unsigned column) const;
		glm::vec3&			get(unsigned row, unsigned column);
		const glm::vec3&	get(unsigned row, unsigned column) const;
		
		unsigned rows() const;
		unsigned columns() const;
		
		void resize(unsigned rows, unsigned cols);
		void build_mesh();
		void draw();
		
		separated_mesh& mesh();
};

#endif