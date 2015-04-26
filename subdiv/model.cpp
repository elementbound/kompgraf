#include "model.h"

typedef model::vertex_t 	vertex_t;
typedef model::edge_t 		edge_t;
typedef model::face_t 		face_t;
typedef model::index_t 		index_t;
typedef model::indexSet_t 	indexSet_t;

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

index_t model::addFaceWithEdges(index_t e1, index_t e2, index_t e3) {
	index_t retId = genFaceIndex();
	edge_t edges[3] = {getEdge(e1), getEdge(e2), getEdge(e3)};
	
	face_t f;
	f.vertices = {edges[0].first, edges[1].first, edges[2].first};
	f.edges = {e1, e2, e3};

	 m_Faces.insert({retId, f});
	 return retId;
}

//

index_t model::findVertex(vertex_t v, float posTolerance, float normalTolerance) {
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

indexSet_t model::findEdgesWithVertex(index_t v) const {
	indexSet_t retSet;

	for(const auto& p : m_Edges) {
		const index_t& index = p.first;
		const edge_t& edge = p.second;

		if(edge.first == v || edge.second == v)
			retSet.insert(index);
	}

	return retSet;
}

indexSet_t model::findFacesWithEdge(index_t e) const {
	indexSet_t retSet;

	for(const auto& p : m_Faces)
	{
		const index_t& index = p.first;
		const face_t& face = p.second;

		if(face.edges[0] == e || face.edges[1] == e || face.edges[2] == e)
			retSet.insert(index);
	}

	return retSet;
}

//

vertex_t& model::getVertex(index_t ind) {
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

//

index_t model::nextVertexIndex(index_t ind) const {
	if(ind == 0)
		return m_Vertices.begin()->first;

	auto it = m_Vertices.upper_bound(ind);
	if(it == m_Vertices.end())
		return 0;
	else
		return it->first;
}

index_t model::nextEdgeIndex(index_t ind) const {
	if(ind == 0)
		return m_Edges.begin()->first;

	auto it = m_Edges.upper_bound(ind);
	if(it == m_Edges.end())
		return 0;
	else
		return it->first;
}

index_t model::nextFaceIndex(index_t ind) const {
	if(ind == 0)
		return m_Faces.begin()->first;

	auto it = m_Faces.upper_bound(ind);
	if(it == m_Faces.end())
		return 0;
	else
		return it->first;
}