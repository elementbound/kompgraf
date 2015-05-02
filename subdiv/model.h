#ifndef _H_MODEL_H_
#define _H_MODEL_H_

#include <glm/glm.hpp>

#include "glwrap/mesh.h"

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

		bool operator<(const vertex_t&) const;
	};

	typedef std::pair<index_t, index_t> edge_t;

	struct face_t
	{
		//initializer-list is king
		std::array<index_t, 3> vertices;
		std::array<index_t, 3> edges;

		bool operator<(const face_t&) const;
	};

	typedef std::set<vertex_t> vertexSet_t;
	typedef std::set<edge_t> edgeSet_t;
	typedef std::set<face_t> faceSet_t;

	private: 
		//NOTE: index 0 is reserved for non-existent items
		std::map<index_t, vertex_t>	m_Vertices;
		std::map<index_t, edge_t>	m_Edges;
		std::map<index_t, face_t>	m_Faces;

		index_t genVertexIndex() const;
		index_t genEdgeIndex() const;
		index_t genFaceIndex() const;

		bool areVerticesEqual(const vertex_t& v1, const vertex_t& v2, float posTolerance, float normalTolerance) const;
		bool areVerticesEqual(index_t v1, index_t v2, float posTolerance, float normalTolerance) const;

	public: 
		float defaultPositionTolerance = 1e-4f;
		float defaultNormalTolerance = 0.05f;

		index_t addVertex(vertex_t v, bool checkForDuplicates = 0);
		index_t addEdge(index_t v1, index_t v2, bool checkForDuplicates = 0);
		index_t addFace(index_t v1, index_t v2, index_t v3);
		index_t addFace(vertex_t v1, vertex_t v2, vertex_t v3, bool checkForDuplicates = 0);
		index_t addFaceWithEdges(index_t e1, index_t e2, index_t e3);

		index_t findVertex(vertex_t v, float posTolerance = -1.0f, float normalTolerance = -1.0f);
		index_t findEdge(index_t v1, index_t v2, bool orderMatters = 0);

		indexSet_t findEdgesWithVertex(index_t v) const;
		indexSet_t findFacesWithEdge(index_t e) const;
		indexSet_t findFacesWithVertex(index_t v) const;

		indexSet_t extractVerticesFromEdges(const indexSet_t&) const;
		indexSet_t extractVerticesFromFaces(const indexSet_t&) const;
		indexSet_t extractEdgesFromFaces(const indexSet_t&) const;

		vertexSet_t extractVerticesFromIndices(const indexSet_t&) const;
		edgeSet_t	extractEdgesFromIndices(const indexSet_t&) const;
		faceSet_t	extractFacesFromIndices(const indexSet_t&) const;

		void removeDuplicateVertices(float posTolerance = -1.0f, float normalTolerance = -1.0f);
		void removeDuplicateEdges(bool orderMatters = 0);

		std::pair<glm::vec3, glm::vec3> getBounds() const;

		vertex_t&	getVertex(index_t ind);
		edge_t&		getEdge(index_t ind);
		face_t&		getFace(index_t ind);

		const vertex_t&		getVertex(index_t ind) const;
		const edge_t&		getEdge(index_t ind) const;
		const face_t&		getFace(index_t ind) const;

		index_t nextVertexIndex(index_t ind) const;
		index_t nextEdgeIndex(index_t ind) const;
		index_t nextFaceIndex(index_t ind) const;

		unsigned vertexCount() const;
		unsigned edgeCount() const;
		unsigned faceCount() const;

		//

		void clear();
};

model loadModelFromOBJ(std::istream& is);
model loadModelFromOBJ(const char* fname);

void buildMeshFromModel(const model& inputModel, basic_mesh& result);
void buildWireframeFromModel(const model& inputModel, basic_mesh& result);

#endif