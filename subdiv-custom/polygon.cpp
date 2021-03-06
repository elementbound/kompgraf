#include "polygon.h"

std::vector<glm::vec2>& 		polygon::data() {return m_Data;}
const std::vector<glm::vec2>& 	polygon::data() const {return m_Data;}

glm::vec2& 			polygon::get(unsigned i) { return m_Data[i];}
const glm::vec2&	polygon::get(unsigned i) const { return m_Data[i];}
glm::vec2&			polygon::operator()(unsigned i) {return this->get(i);}
const glm::vec2&	polygon::operator()(unsigned i) const {return this->get(i);}
glm::vec2&			polygon::operator[](unsigned i) {return this->get(i);}
const glm::vec2&	polygon::operator[](unsigned i) const {return this->get(i);}

unsigned polygon::size() const {return m_Data.size();}
void polygon::resize(unsigned s) {m_Data.resize(s);}

void polygon::clear() {m_Data.clear();}
void polygon::add(glm::vec2 p) {m_Data.push_back(p);}