#ifndef _H_SHADER_H_
#define _H_SHADER_H_

#include <GL/glew.h>
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
		
		GLuint handle();
};

#endif