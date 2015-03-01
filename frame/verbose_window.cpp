#include "verbose_window.h"
#include <iostream>

void verbose_window::on_reposition(int x, int y){std::cout << this << ": Reposition" << std::endl;}
void verbose_window::on_resize(int w, int h){std::cout << this << ": Resize" << std::endl;}
void verbose_window::on_fbresize(int w, int h){std::cout << this << ": Framebuffer resize" << std::endl;}
void verbose_window::on_refresh(){std::cout << this << ": Refresh" << std::endl;}
void verbose_window::on_focus(bool focused){std::cout << this << ": Focus" << std::endl;}
void verbose_window::on_minimize(bool minimized){std::cout << this << ": Minimize" << std::endl;}

void verbose_window::on_open(){std::cout << this << ": Open" << std::endl;}
void verbose_window::on_close(){std::cout << this << ": Close" << std::endl;}

void verbose_window::on_mousebutton(int button, int action, int mods){std::cout << this << ": Mouse button" << std::endl;}
void verbose_window::on_mousepos(double x, double y){std::cout << this << ": Mouse position" << std::endl;}
void verbose_window::on_mousefocus(bool focused){std::cout << this << ": Mouse focus, " << focused << std::endl;}
void verbose_window::on_mousescroll(double x, double y){std::cout << this << ": Mouse scroll, " << x << ", " << y << std::endl;}
void verbose_window::on_key(int key, int scancode, int action, int mods){std::cout << this << ": Key" << std::endl;}
void verbose_window::on_char(unsigned int code){std::cout << this << ": Character: " << char(code) << std::endl;}