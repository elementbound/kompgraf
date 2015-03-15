#ifndef _H_CONTROL_POLY_H_
#define _H_CONTROL_POLY_H_ 

#include <glm/glm.hpp>
#include <vector>

class control_poly 
{
	protected: 
		std::vector<glm::vec2> m_Data; 
		
	public: 
		glm::vec2& 			get(unsigned i);
		const glm::vec2&	get(unsigned i) const;
		glm::vec2&			operator()(unsigned i);
		const glm::vec2&	operator()(unsigned i) const;
		glm::vec2&			operator[](unsigned i);
		const glm::vec2&	operator[](unsigned i) const;
		
		unsigned size() const;
		void resize(unsigned s);
		
		void clear();
		void add(glm::vec2 p);
};

#endif