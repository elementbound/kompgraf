#ifndef _H_DRAW_CIRCLES_H_
#define _H_DRAW_CIRCLES_H_

#include "glwrap/mesh.h"

class drawCircles
{
	private: 
		static separated_mesh m_Mesh;
		static unsigned pos;
		
	public: 
		drawCircles() = delete;
		drawCircles(const drawCircles&) = delete;
		drawCircles(drawCircles&&) = delete;
		~drawCircles() = delete;
	
		static void begin(bool outline);
		static void add_circle(float x, float y, float r, unsigned detail = 16);
		static void end();
		
		static void single_circle(float x, float y, float r, bool outline, unsigned detail = 16);
};

#endif