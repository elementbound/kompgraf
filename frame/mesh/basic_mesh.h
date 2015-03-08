#ifndef _H_BASIC_MESH_H_
#define _H_BASIC_MESH_H_

#include <GL/glew.h>
#include <map>
#include <string>
#include "frame/buffer.hpp"

class basic_mesh
{
	public: 
		struct stream_data
		{
			std::string name; 
			GLenum type;
			GLenum buffer_type;
			unsigned components;
			bool normalized;
			
			buffer data;
		};
		
		GLenum draw_mode;
		GLenum storage_policy;
		
		basic_mesh();
		basic_mesh(const basic_mesh&) = delete;
		~basic_mesh();
		
		basic_mesh& operator=(const basic_mesh&) = delete;
	
		unsigned add_stream();
		void	 clear_streams();
		bool     remove_stream(unsigned id);
		
		stream_data& get_stream(unsigned id);
		const stream_data& get_stream(unsigned id) const;
		stream_data& operator[](unsigned id);
		const stream_data& operator[](unsigned id) const;
		
		unsigned stream_count() const;
		
		virtual void upload() = 0;
		virtual void bind() = 0;
		virtual void draw() = 0;
		
	protected: 
		std::map<unsigned, stream_data> m_Streams;
		GLuint m_VAO;
};

#endif