#include "spline_surface.h"
#include "frame/util.h"

glm::vec3 spline_surface::eval(float u, float v) const
{
	glm::vec3 p = glm::vec3(0.0f);
	
	for(unsigned row = 0; row < control_data.rows(); row++)
		for(unsigned col = 0; col < control_data.columns(); col++)
			p += this->weight(u,v, row,col) * control_data(row, col);
		
	return p;
}

void spline_surface::build_grid(unsigned detail, unsigned density)
{
	m_GridMesh.clear_streams();
	m_GridMesh.storage_policy = GL_DYNAMIC_DRAW;
	m_GridMesh.draw_mode = GL_LINES;

	unsigned pos = m_GridMesh.add_stream();
	m_GridMesh[pos].type = GL_FLOAT;
	m_GridMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_GridMesh[pos].components = 3;
	m_GridMesh[pos].normalized = 0;
	m_GridMesh[pos].name = "vertexPosition";

	//Rows
	for(unsigned row = 0; row < density; row++)
	{
		for(unsigned x=0; x+1<=detail; x++)
		{
			float u[2] = {x/(float)detail, (x+1)/(float)detail};
			float v = row/(float)(density-1);
			m_GridMesh[pos].data << this->eval(u[0], v);
			m_GridMesh[pos].data << this->eval(u[1], v);
		}
	}

	//Columns
	for(unsigned column = 0; column < density; column++)
	{
		for(unsigned y=0; y+1<=detail; y++)
		{
			float u = column/(float)(density-1);
			float v[2] = {y/(float)detail, (y+1)/(float)detail};
			m_GridMesh[pos].data << this->eval(u, v[0]);
			m_GridMesh[pos].data << this->eval(u, v[1]);
		}
	}

	//

	m_GridMesh.upload();
}

void spline_surface::build_eval(unsigned detail)
{
	m_EvalMesh.clear_streams();
	m_EvalMesh.storage_policy = GL_DYNAMIC_DRAW;
	m_EvalMesh.draw_mode = GL_TRIANGLES;
	
	unsigned pos = m_EvalMesh.add_stream();
	m_EvalMesh[pos].type = GL_FLOAT;
	m_EvalMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_EvalMesh[pos].components = 3;
	m_EvalMesh[pos].normalized = 0;
	m_EvalMesh[pos].name = "vertexPosition";
	
	unsigned nor = m_EvalMesh.add_stream();
	m_EvalMesh[nor].type = GL_FLOAT;
	m_EvalMesh[nor].buffer_type = GL_ARRAY_BUFFER;
	m_EvalMesh[nor].components = 3;
	m_EvalMesh[nor].normalized = 0;
	m_EvalMesh[nor].name = "vertexNormal";
	
	float nabla = 1.0f/float(detail-1);
	static std::map<std::pair<unsigned,unsigned>, glm::vec3> pos_map;
	static std::map<std::pair<unsigned,unsigned>, glm::vec3> nor_map; //normals
	
	for(unsigned y=0; y<detail; y++)
	{
		for(unsigned x=0; x<detail; x++)
		{
			float u = x/float(detail-1);
			float v = y/float(detail-1);
						 
			glm::vec3 p_at = this->eval(u,v);
			
			glm::vec3 p_west = this->eval(u-nabla,v);
			glm::vec3 p_north = this->eval(u,v-nabla);
			glm::vec3 p_east = this->eval(u+nabla,v);
			glm::vec3 p_south = this->eval(u,v+nabla);
			
			glm::vec3 normal = glm::normalize(glm::cross(glm::normalize(p_east-p_west), glm::normalize(p_south-p_north)));
			
			pos_map.insert({{x,y}, p_at});
			nor_map.insert({{x,y}, normal});
		}
	}
	
	for(unsigned y=0; y+1<detail; y++)
	{
		for(unsigned x=0; x+1<detail; x++)
		{
			//Top-left 
			m_EvalMesh[pos].data << pos_map[{x  ,y  }];
			m_EvalMesh[nor].data << nor_map[{x  ,y  }];
			
			//Top-right
			m_EvalMesh[pos].data << pos_map[{x+1,y  }];
			m_EvalMesh[nor].data << nor_map[{x+1,y  }];
			
			//Bottom-left
			m_EvalMesh[pos].data << pos_map[{x  ,y+1}];
			m_EvalMesh[nor].data << nor_map[{x  ,y+1}];
			
			//
			
			//Top-right
			m_EvalMesh[pos].data << pos_map[{x+1,y  }];
			m_EvalMesh[nor].data << nor_map[{x+1,y  }];
			
			//Bottom-left
			m_EvalMesh[pos].data << pos_map[{x  ,y+1}];
			m_EvalMesh[nor].data << nor_map[{x  ,y+1}];
			
			//Bottom-right
			m_EvalMesh[pos].data << pos_map[{x+1,y+1}];
			m_EvalMesh[nor].data << nor_map[{x+1,y+1}];
		}
	}
	
	pos_map.clear();
	nor_map.clear();
	
	m_EvalMesh.upload();
}

separated_mesh& spline_surface::eval_mesh() 
{
	return m_EvalMesh;
}

separated_mesh& spline_surface::grid_mesh()
{
	return m_GridMesh;
}