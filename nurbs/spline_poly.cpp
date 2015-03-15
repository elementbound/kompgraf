#include "spline_poly.h"
#include "frame/util.h"
#include <cmath>
#include <iostream>

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

//===========================================================================================//

float bspline_poly::weight(float t, unsigned i) const 
{
	if(order != 3)
		return 0.0f; //bspline > Tom ;-;
	
	switch(i)
	{
		case 0: 
			return 1.0f/6.0f * pow(1.0-t, 3);
			
		case 1: 
			return 1.0/2.0 * pow(t, 3) - pow(t,2) + 2.0/3.0;
			
		case 2: 
			return -1.0/2.0 * pow(t, 3) + 1.0/2.0 * pow(t,2) + 1.0/2.0 * t + 1.0/6.0;
			
		case 3: 
			return 1.0/6.0 * pow(t, 3);
			
		default: 
			return 0.0f;
	}
}

glm::vec2 bspline_poly::eval(float t) const
{
	std::cout << "bspline_poly::eval(" << t << "): " << (t * ( control_data->size() - order )) << " / ";
	
	glm::vec2 p = glm::vec2(0.0f);
	unsigned offs = std::floor(t * ( control_data->size() - order ));
	t = std::fmod(t, 1.0f);
	
	std::cout << offs << " / " << t << std::endl;
	
	for(unsigned i = 0; i < order+1; i++)
		p += this->weight(t, i) * control_data->get((i+offs) % control_data->size());
	
	return p;
}