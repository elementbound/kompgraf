#include "spline_poly.h"
#include "frame/util.h"

glm::vec2 spline_poly::eval(float t) const
{
	glm::vec2 p = glm::vec2(0.0f);
	
	for(unsigned i = 0; i < control_data->size(); i++)
		p += this->weight(t, i) * control_data->get(i);
		
	return p;
}

void spline_poly::build_eval(unsigned detail)
{
	m_EvalMesh.clear_streams();
	m_EvalMesh.storage_policy = GL_DYNAMIC_DRAW;
	m_EvalMesh.draw_mode = GL_LINE_STRIP;
	
	unsigned pos = m_EvalMesh.add_stream();
	m_EvalMesh[pos].type = GL_FLOAT;
	m_EvalMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_EvalMesh[pos].components = 2;
	m_EvalMesh[pos].normalized = 0;
	m_EvalMesh[pos].name = "vertexPosition";
	
	for(unsigned i=0; i<detail; i++)
	{
		float t = i/float(detail-1);
		m_EvalMesh[pos].data << this->eval(t);
	}
	
	m_EvalMesh.upload();
}

separated_mesh& spline_poly::eval_mesh() 
{
	return m_EvalMesh;
}

//===========================================================================================//

float bezier_poly::weight(float t, unsigned i) const 
{
	return float(std::pow(t,i)*std::pow(1.0f-t, control_data->size()-1-i)*combi(control_data->size()-1, i));
}

float bspline_poly::weight(float t, unsigned i) const 
{
	return ( i/(float)control_data->size() <= t && t < (i+1)/(float)control_data->size() );
}