#include <cmath>
#include <cstdlib> //rand
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>

#include "frame/window.h"
#include <iostream>

double degtorad(double x){return x/180.0 * 3.14159265;}
double randd(){return rand()/(double)RAND_MAX;}
double birand(){return -1.0 + 2.0*randd();}

void glcCircle(double x, double y, double r, bool outline, unsigned det = 16)
{
	glBegin(outline ? GL_LINE_STRIP : GL_TRIANGLE_FAN);
		for(unsigned i=0; i<=det; i++)
		{
			double angle = i/(double)det * 2.0 * 3.14159265;
			glVertex2d(x + cos(angle)*r, y - sin(angle)*r);
		}
	glEnd();
}

class editable_polygon
{
	public:
		std::vector<glm::vec2> points;
		bool isClosed;
		
		double circle_radius;
		int dragged_id;
		
		void init()
		{
			circle_radius = 1.0/32.0;
			dragged_id = -1;
		}
		
		void mousebutton(glm::vec2 mouse_pos, int button, int action)
		{
			if(action == GLFW_PRESS)
			{
				dragged_id = -1;
				
				for(unsigned i=0; i<points.size(); i++)
					if(glm::distance2(mouse_pos, points[i]) < circle_radius/2.0)
					{
						dragged_id = i;
						break;
					}
			}
			
			if(action == GLFW_RELEASE)
				dragged_id = -1;
		}
		
		void update(glm::vec2 mouse_pos)
		{
			if(dragged_id >= 0)
				points[dragged_id] = mouse_pos;
		}
		
		void draw()
		{
			glBegin(isClosed ? GL_LINE_LOOP : GL_LINE_STRIP);
				for(glm::vec2& p : points)
					glVertex2d(p.x, p.y);
			glEnd();
			
			for(unsigned i=0; i<points.size(); i++)
					glcCircle(points[i].x, points[i].y, circle_radius, i != dragged_id);
		}
		
		float weight(float t, unsigned i, unsigned p, unsigned n)
		{
			/*auto ti = [n](unsigned i) -> float {return i/(float)(n-1);};
			
			if(p == 0)
				return (ti(i) <= t && t < ti(i+1)) ? 1.0f : 0.0;
			else 
				return (t - ti(i))/(ti(i+p) - ti(i)) * weight(t, i, p-1, n) + 
					   (ti(i+p+1) - t)/(ti(i+p+1) - ti(i+1)) * weight(t, i+1, p-1, n);*/
			
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
		
		glm::vec2 eval(float t, unsigned start, unsigned order)
		{
			glm::vec2 p = glm::vec2(0.0);
			for(unsigned i=0; i<order; i++)
				p += weight(t, i, order, points.size()) * points[(start+i) % points.size()];
			
			return p;
		}
		
		void draw_bspline(unsigned det)
		{
			if(points.size() < 4)
				return;
			
			for(int i=0; i<points.size() - (isClosed ? 0 : 3); i++)
			{
				glm::vec2 eval_point;
				
				glBegin(GL_LINE_STRIP);
				for(unsigned j=0; j<det; j++)
				{
					double t = j/(double)(det-1);
					
					glm::vec2 eval_point = this->eval(t, i, 4);
					glVertex2d(eval_point.x, eval_point.y);
				}
				glEnd();
			}
		}
};

class app_window : public window
{
	private: 
		double		m_Aspect;
		glm::mat4	m_Projection;
		glm::vec2	m_Mouse;
		
		int m_Width, m_Height;
		
		editable_polygon m_Polygon;
	
	protected:
		void on_open()
		{
			int w, h;
			glfwGetFramebufferSize(this->handle(), &w, &h);
			this->on_fbresize(w,h);
			
			glClearColor(0.0, 0.0, 0.0, 1.0);
			
			glMatrixMode(GL_PROJECTION);
			glOrtho(-1,1, -1,1, -1,1);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			//
			
			m_Polygon.init();
		}
		
		void on_fbresize(int w, int h)
		{
			m_Width = w;
			m_Height = h;
			
			m_Aspect = (double)m_Width / m_Height;
			glViewport(0,0, w,h);
			
			glMatrixMode(GL_PROJECTION);
			m_Projection = glm::ortho(-m_Aspect,m_Aspect, -1.0,1.0);
			glLoadMatrixf(glm::value_ptr(m_Projection));
		}
		
		void on_mousepos(double x, double y)
		{
			m_Mouse = glm::vec2(x,y);
			m_Mouse = glm::vec2( glm::unProject(glm::vec3(m_Mouse.x, m_Height - m_Mouse.y, 1.0), glm::mat4(1.0), m_Projection, glm::vec4(0.0, 0.0, m_Width, m_Height)) );
		}
		
		void on_mousebutton(int button, int action, int mods)
		{
			m_Polygon.mousebutton(m_Mouse, button, action);
		}
		
		void on_key(int key, int scancode, int action, int mods)
		{
			switch(key)
			{
				case GLFW_KEY_ESCAPE:
					glfwSetWindowShouldClose(this->handle(), 1);
				break;
					
				case GLFW_KEY_Q:
					if(action == GLFW_PRESS)
						m_Polygon.points.push_back(m_Mouse);
				break;
					
				case GLFW_KEY_W:
					if(action == GLFW_PRESS)
					{
						if(m_Polygon.isClosed)
							m_Polygon.isClosed = 0;
						else 
							m_Polygon.isClosed = 1;
					}
				break;
			}
		}
		
		void on_refresh()
		{
			glClear(GL_COLOR_BUFFER_BIT);
			
			m_Polygon.update(m_Mouse);
			
			glcCircle(m_Mouse.x, m_Mouse.y, 1.0/32, 1);
			m_Polygon.draw();
			m_Polygon.draw_bspline(64);
			
			glfwSwapBuffers(handle());
		}
};

int main()
{
	if(!glfwInit())
		return 1;

	app_window wnd;
	wnd.open(512,512, "Dem test tho");
	wnd.make_current();
	
	glfwSwapInterval(0);
	
	GLenum err = glewInit();
	if(err != GLEW_OK)
		return 2;
	
	while(!glfwWindowShouldClose(wnd()))
	{
		wnd.refresh();
		glfwPollEvents();
	}

	return 0;
}