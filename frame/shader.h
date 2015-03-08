#ifndef _H_SHADER_H_
#define _H_SHADER_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class shader_program
{
	public: 
		enum shader_type 
		{
			invalid, 
			vertex, 
			fragment, 
			geometry
		};
		
	private: 
		struct shader
		{
			GLuint gl_id = 0;
			shader_type type = shader_type::invalid;
			
			void source(const char* src, shader_type type);
			bool compile();
			std::string log();
		};
		
		std::vector<shader> m_Shaders;
		GLuint m_Program;
		
	public:
		void create();
		bool attach(const char* src, shader_type type);
		//TODO: glBindFragDataLocation
		void link();
		void use();
		
		int  uniform(const char* name) const;
		int  uniform(const std::string& name) const;
		
		void set_uniform(int loc, glm::vec2 value) const;
		void set_uniform(int loc, glm::vec3 value) const;
		void set_uniform(int loc, glm::vec4 value) const;
		void set_uniform(int loc, glm::mat4 value) const;
		
		void set_uniform(const char* name, glm::vec2 value) const;
		void set_uniform(const char* name, glm::vec3 value) const;
		void set_uniform(const char* name, glm::vec4 value) const;
		void set_uniform(const char* name, glm::mat4 value) const;
		
		void set_uniform(const std::string& name, glm::vec2 value) const;
		void set_uniform(const std::string& name, glm::vec3 value) const;
		void set_uniform(const std::string& name, glm::vec4 value) const;
		void set_uniform(const std::string& name, glm::mat4 value) const;
		
		GLuint handle() const;
};

#endif