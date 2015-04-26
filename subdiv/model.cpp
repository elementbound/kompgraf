#include "model.h"

using model::vertex_t;
using model::edge_t;
using model::face_t;
using model::index_t;

index_t model::genVertexIndex() const {
	if(m_Vertices.empty())
		return 1;
	else
		return (*m_Vertices.rbegin()).first + 1;
}

index_t model::genEdgeIndex() const {
	if(m_Edges.empty())
		return 1;
	else
		return (*m_Edges.rbegin()).first + 1;
}

index_t model::genFaceIndex() const {
	if(m_Faces.empty())
		return 1;
	else
		return (*m_Faces.rbegin()).first + 1;
}

//

index_t model::addVertex(vertex_t v, bool checkForDuplicates) {
	index_t retId = 0;
	bool addVertex = true;

	if(checkForDuplicates) {
		retId = findVertex(v);

		//Vertex already exists in model
		if(retId != 0)
			addVertex = false;
	}

	if(addVertex) {
		retId = genVertexIndex();
		m_Vertices.insert({retId, v});
	}

	return retId;
}

index_t model::addEdge(index_t v1, index_t v2, bool checkForDuplicates) {
	index_t retId = 0;
	bool addEdge = false;

	if(checkForDuplicates) {
		retId = findEdge(v1, v2);

		if(retId != 0)
			addEdge = false;
	}

	if(addEdge) {
		retId = genEdgeIndex();
		m_Edges.insert({retId, edge_t(v1, v2)});
	}

	return retId;
}

index_t model::addFace(index_t v1, index_t v2, index_t v3) {
	index_t retId = genFaceIndex();
	face_t f; 

	f.vertices = {v1, v2, v3};
	f.edges = {addEdge(v1, v2, true), addEdge(v2, v3, true), addEdge(v3, v1, true)};

	m_Faces.insert({retId, f});
	return retId;
}

index_t model::addFace(index_t e1, index_t e2, index_t e3) {
	index_t retId = genFaceIndex();
	edge_t edges[3] = {getEdge(e1), getEdge(e2), getEdge(e3)};
	
	face_t f;
	f.vertices = {edges[0].first, edges[1].first, edges[2].first};
	f.edges = {e1, e2, e3};

	 m_Faces.insert({retId, f});
	 return retId;
}

index_t model::addFace(vertex_t v1, vertex_t v2, vertex_t v3) {
	index_t retId = genFaceIndex();
	face_t f;
	f.vertices = {addVertex(v1, true), addVertex(v2, true), addVertex(v3, true)};
	f.edges = {	addEdge(f.vertices[0], f.vertices[1], true), 
				addEdge(f.vertices[1], f.vertices[2], true), 
				addEdge(f.vertices[2], f.vertices[0], true) };

	m_Faces.insert({retId, f});
	return retId;
}

//

index_t model::findVertex(vertex_t v, float posTolerance = 1e-4f, float normalTolerance = 1e-4f) {
	for(const std::pair<index_t, vertex_t>& p : m_Vertices) {
		if(glm::length(v.position - p.second.position) < posTolerance && 
			glm::abs(glm::dot(v.normal, p.second.normal)) < normalTolerance )
			return p.first;
	}

	return 0;
}

index_t model::findEdge(index_t v1, index_t v2) {
	edge_t needle = edge_t(v1, v2);

	for(const std::pair<index_t, edge_t>& p : m_Edges)
	{
		if(needle == p.second)
			return p.first;
	}

	return 0;
}

//

vertex_t model::getVertex(index_t ind) {
	return m_Vertices.at(ind);
}
edge_t& model::getEdge(index_t ind) {
	return m_Edges.at(ind);
}
face_t& model::getFace(index_t ind) {
	return m_Faces.at(ind);
}

const vertex_t& model::getVertex(index_t ind) const {
	return m_Vertices.at(ind);
}

const edge_t& model::getEdge(index_t ind) const {
	return m_Edges.at(ind);
}

const face_t& model::getFace(index_t ind) const {
	return m_Faces.at(ind);
}