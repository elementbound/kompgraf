#include "control_poly.h"

glm::vec2& 			control_poly::get(unsigned i) { return m_Data[i];}
const glm::vec2&	control_poly::get(unsigned i) const { return m_Data[i];}
glm::vec2&			control_poly::operator()(unsigned i) {return this->get(i);}
const glm::vec2&	control_poly::operator()(unsigned i) const {return this->get(i);}
glm::vec2&			control_poly::operator[](unsigned i) {return this->get(i);}
const glm::vec2&	control_poly::operator[](unsigned i) const {return this->get(i);}

unsigned control_poly::size() const {return m_Data.size();}
void control_poly::resize(unsigned s) {m_Data.resize(s);}

void control_poly::clear() {m_Data.clear();}
void control_poly::add(glm::vec2 p) {m_Data.push_back(p);}