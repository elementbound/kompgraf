#include "polygon.h"

glm::vec2& 			polygon::get(unsigned i) { return m_Data[i];}
const glm::vec2&	polygon::get(unsigned i) const { return m_Data[i];}
glm::vec2&			polygon::operator()(unsigned i) {return this->get(i);}
const glm::vec2&	polygon::operator()(unsigned i) const {return this->get(i);}
glm::vec2&			polygon::operator[](unsigned i) {return this->get(i);}
const glm::vec2&	polygon::operator[](unsigned i) const {return this->get(i);}

unsigned polygon::size() const {return m_Data.size();}
void polygon::resize(unsigned s) {m_Data.resize(s); m_Weights.resize(s);}

void polygon::clear() {m_Data.clear(); m_Weights.clear();}
void polygon::add(glm::vec2 p) {m_Data.push_back(p);}