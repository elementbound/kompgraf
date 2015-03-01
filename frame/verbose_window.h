#ifndef _H_VERBOSE_WINDOW_H_
#define _H_VERBOSE_WINDOW_H_

#include "window.h"

class verbose_window : public window
{
	protected:
		void on_reposition(int x, int y);
		void on_resize(int w, int h);
		void on_fbresize(int w, int h);
		void on_refresh();
		void on_focus(bool focused);
		void on_minimize(bool minimized);
		
		void on_open();
		void on_close();
		
		void on_mousebutton(int button, int action, int mods);
		void on_mousepos(double x, double y);
		void on_mousefocus(bool focused);
		void on_mousescroll(double x, double y);
		void on_key(int key, int scancode, int action, int mods);
		void on_char(unsigned int code);
};

#endif