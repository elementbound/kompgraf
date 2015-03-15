#include <GL/glew.h>
#include "resizable_window.h"

void resizable_window::on_resize(int w, int h)
{
	m_WindowWidth = w;
	m_WindowHeight = h;
	
	m_WindowAspect = w/(double)h;
	
	glViewport(0,0, m_WindowWidth,m_WindowHeight);
}

void resizable_window::on_fbresize(int w, int h)
{
	m_FramebufferWidth = w;
	m_FramebufferHeight = h;
	
	m_FramebufferAspect = w/(double)h;
}