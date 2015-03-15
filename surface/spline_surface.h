#ifndef _H_SPLINE_SURFACE_H_
#define _H_SPLINE_SURFACE_H_

#include <GLM/glm.hpp>
#include "frame/mesh.h"
#include "control_surface.h"

class spline_surface
{
	private: 
		separated_mesh 		m_EvalMesh;
		separated_mesh		m_GridMesh;
		
	public: 
		control_surface		control_data;
		
		virtual float 		weight(float u, float v, unsigned row, unsigned col) const = 0;
		glm::vec3 			eval(float u, float v) const;
		
		void 				build_grid(unsigned detail, unsigned density);
		void 				build_eval(unsigned detail);
		
		separated_mesh& 	eval_mesh();
		separated_mesh& 	grid_mesh();
};

class bezier_surface: public spline_surface
{
	public: 
		float weight(float u, float v, unsigned row, unsigned col) const;
};

class bspline_surface: public spline_surface 
{
	public: 
		unsigned order = 2;
		
		float weight(float u, float v, unsigned row, unsigned col) const;
};

#endif