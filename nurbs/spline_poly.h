#ifndef _H_SPLINE_POLY_H_
#define _H_SPLINE_POLY_H_

#include <GLM/glm.hpp>
#include "frame/mesh.h"
#include "control_poly.h"

class spline_poly
{
	private: 
		separated_mesh 		m_EvalMesh;
		
	public: 
		control_poly*		control_data = NULL;
		
		virtual float 		weight(float t, unsigned i) const = 0;
		glm::vec2 			eval(float t) const;
		
		void 				build_eval(unsigned detail);
		
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
		
		float weight(float t, unsigned i) const;
};

#endif