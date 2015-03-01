#ifndef _H_WINDOW_H_
#define _H_WINDOW_H_

#include <GLFW/glfw3.h>
#include <map>

class window
{
	private:
		GLFWwindow* m_Window;
		
		static std::map<GLFWwindow*,window*> s_Window;

		static void cb_reposition(GLFWwindow*, int, int);
		static void cb_resize(GLFWwindow*, int, int);
		static void cb_close(GLFWwindow*);
		static void cb_refresh(GLFWwindow*);
		static void cb_focus(GLFWwindow*, int);
		static void cb_iconify(GLFWwindow*, int);
		static void cb_fbresize(GLFWwindow*, int, int);
		
		static void cb_mousebutton(GLFWwindow*, int, int, int);
		static void cb_mousepos(GLFWwindow*, double, double);
		static void cb_mousefocus(GLFWwindow*, int);
		static void cb_mousescroll(GLFWwindow*, double, double);
		static void cb_key(GLFWwindow*, int, int, int, int);
		static void cb_char(GLFWwindow*, unsigned int);
		
		static void cb_unknown_window(GLFWwindow*);
		
	protected:
		virtual void on_reposition(int x, int y);
		virtual void on_resize(int w, int h);
		virtual void on_fbresize(int w, int h);
		virtual void on_refresh();
		virtual void on_focus(bool focused);
		virtual void on_minimize(bool minimized);
		
		virtual void on_open();
		virtual void on_close();
		
		virtual void on_mousebutton(int button, int action, int mods);
		virtual void on_mousepos(double x, double y);
		virtual void on_mousefocus(bool focused);
		virtual void on_mousescroll(double x, double y);
		virtual void on_key(int key, int scancode, int action, int mods);
		virtual void on_char(unsigned int code);
	
	public:
		window();
		window(const window&) = delete;
		//window(window&&); //TODO
		~window();
		
		bool open(int w, int h, const char* title, GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL);
		bool make_current();
		bool close();
		
		void refresh();
		//TODO: event senders
		
		const GLFWwindow* handle() const;
		GLFWwindow* handle();
		const GLFWwindow* operator()() const;
		GLFWwindow* operator()();
		
		bool is_valid() const;
		operator bool () const;
};

#endif