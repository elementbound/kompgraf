#include "spline_poly.h"
#include "frame/util.h"
#include <cmath>
#include <iostream>
#include <algorithm> //std::swap

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
	
	unsigned len = m_EvalMesh.add_stream();
	m_EvalMesh[len].type = GL_FLOAT;
	m_EvalMesh[len].buffer_type = GL_ARRAY_BUFFER;
	m_EvalMesh[len].components = 1;
	m_EvalMesh[len].normalized = 0;
	m_EvalMesh[len].name = "vertexDistance";
	
	glm::vec2 p[2] = {glm::vec2(0.0f), this->eval(0.0f)};
	float vertex_distance = 0.0;
	
	for(unsigned i=0; i<detail; i++)
	{
		float t = i/float(detail-1);
		p[0] = this->eval(t);
		
		vertex_distance += glm::distance(p[0], p[1]);
		std::swap(p[0], p[1]);
		
		m_EvalMesh[pos].data << p[0];
		m_EvalMesh[len].data << vertex_distance;
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

glm::vec2 bspline_poly::eval(float t, unsigned i) const
{
	glm::vec2 p = glm::vec2(0.0f);
	
	for(unsigned j = 0; j < order+1; j++)
		p += this->weight(t, j) * control_data->get((i+j) % control_data->size());
	
	return p;
}

void bspline_poly::build_eval(unsigned detail)
{
	if(control_data->size() < order+1)
		return;
	
	m_EvalMesh.clear_streams();
	m_EvalMesh.storage_policy = GL_DYNAMIC_DRAW;
	m_EvalMesh.draw_mode = GL_LINE_STRIP;
	
	unsigned pos = m_EvalMesh.add_stream();
	m_EvalMesh[pos].type = GL_FLOAT;
	m_EvalMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	m_EvalMesh[pos].components = 2;
	m_EvalMesh[pos].normalized = 0;
	m_EvalMesh[pos].name = "vertexPosition";
	
	for(unsigned i=0; i < control_data->size() - order; i++)
	{
		for(unsigned j=0; j<detail; j++)
		{
			float t = j/float(detail-1);
			m_EvalMesh[pos].data << this->eval(t, i);
		}
	}
	
	m_EvalMesh.upload();
}

//===========================================================================================//

glm::vec2 nurbs_poly::eval(float t, unsigned i) const
{
	glm::vec2 p = glm::vec2(0.0f);
	float weight_sum = 0.0f;
	
	for(unsigned j=0; j < order+1; j++)
	{
		p += this->weight(t, j) * control_data->weight((i+j) % control_data->size()) * 
			 control_data->get((i+j)%control_data->size());
			 
		weight_sum += this->weight(t, j) * control_data->weight((i+j) % control_data->size());
	}
	
	return p/weight_sum;
}