#ifndef _H_RESIZABLE_WINDOW_H_
#define _H_RESIZABLE_WINDOW_H_ 

#include "window.h"

class resizable_window : public window
{
	protected: 
		unsigned m_WindowWidth;
		unsigned m_WindowHeight; 
		
		unsigned m_FramebufferWidth;
		unsigned m_FramebufferHeight;
		
		double   m_WindowAspect; 
		double   m_FramebufferAspect;
		
		void on_resize(int w, int h);
		void on_fbresize(int w, int h);
};

#endif