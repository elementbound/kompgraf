#ifndef _H_SPLINE_POLY_H_
#define _H_SPLINE_POLY_H_

#include <GLM/glm.hpp>
#include "frame/mesh.h"
#include "control_poly.h"

class spline_poly
{
	protected: 
		separated_mesh 		m_EvalMesh;
		
	public: 
		control_poly*		control_data = NULL;
		
		virtual float 		weight(float t, unsigned i) const = 0;
		virtual glm::vec2 	eval(float t) const;
		
		virtual void 		build_eval(unsigned detail);
		
		separated_mesh& 	eval_mesh();
};

class bezier_poly: public spline_poly
{
	public: 
		float weight(float t, unsigned i) const;
};

class bspline_poly: public spline_poly 
{
	public: 
		unsigned order = 3;
		
		virtual float weight(float t, unsigned i) const;
		virtual glm::vec2 eval(float t, unsigned i) const;
		
		virtual void build_eval(unsigned detail);
};

class nurbs_poly: public bspline_poly 
{
	public: 
		glm::vec2 eval(float t, unsigned i) const;
};

#endif