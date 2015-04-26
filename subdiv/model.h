#ifndef _H_MODEL_H_
#define _H_MODEL_H_

#include <glm/glm.hpp>

#include <istream>
#include <map>
#include <set>
#include <array>

class model
{
	public:
	typedef unsigned index_t;
	typedef std::set<index_t> indexSet_t;

	struct vertex_t
	{
		glm::vec3 position;
		glm::vec3 normal;
	};

	typedef std::pair<index_t, index_t> edge_t;

	struct face_t
	{
		//initializer-list is king
		std::array<index_t, 3> vertices;
		std::array<index_t, 3> edges;
	};

	private: 
		//NOTE: index 0 is reserved for non-existent items
		std::map<index_t, vertex_t>	m_Vertices;
		std::map<index_t, edge_t>	m_Edges;
		std::map<index_t, face_t>	m_Faces;

		index_t genVertexIndex() const;
		index_t genEdgeIndex() const;
		index_t genFaceIndex() const;

	public: 
		float defaultPositionTolerance = 1e-4f;
		float defaultNormalTolerance = 0.05f;

		index_t addVertex(vertex_t v, bool checkForDuplicates = 0);
		index_t addEdge(index_t v1, index_t v2, bool checkForDuplicates = 0);
		index_t addFace(index_t v1, index_t v2, index_t v3);
		index_t addFace(vertex_t v1, vertex_t v2, vertex_t v3);
		index_t addFaceWithEdges(index_t e1, index_t e2, index_t e3);
		
		index_t findVertex(vertex_t v, float posTolerance = -1.0f, float normalTolerance = -1.0f);
		index_t findEdge(index_t v1, index_t v2);

		indexSet_t findEdgesWithVertex(index_t v) const;
		indexSet_t findFacesWithEdge(index_t e) const;

		vertex_t&	getVertex(index_t ind);
		edge_t&		getEdge(index_t ind);
		face_t&		getFace(index_t ind);

		const vertex_t&		getVertex(index_t ind) const;
		const edge_t&		getEdge(index_t ind) const;
		const face_t&		getFace(index_t ind) const;

		index_t nextVertexIndex(index_t ind) const;
		index_t nextEdgeIndex(index_t ind) const;
		index_t nextFaceIndex(index_t ind) const;

		//

		void clear();
};

model loadModelFromOBJ(std::istream& is);
model loadModelFromOBJ(const char* fname);

#endif