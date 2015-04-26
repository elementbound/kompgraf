#include "subdiv_operator.h"
#include <vector>

#include "debug.h"

typedef model::vertex_t 	vertex_t;
typedef model::edge_t 		edge_t;
typedef model::face_t 		face_t;

typedef model::index_t 		index_t;
typedef model::indexSet_t 	indexSet_t;

typedef model::vertexSet_t	vertexSet_t;
typedef model::edgeSet_t	edgeSet_t;
typedef model::faceSet_t	faceSet_t;

typedef std::vector<index_t> 	indexSeq_t;

typedef std::vector<vertex_t>	vertexSeq_t;
typedef std::vector<edge_t>		edgeSeq_t;
typedef std::vector<face_t>		faceSeq_t;

model subdivOperator::operator()(const model& inputModel) {
	model resultModel = inputModel;
	return resultModel;
}

model midpointSubdivOperator::operator()(const model& inputModel) {
	model resultModel;
	dbg("Midpoint subdiv, " << inputModel.faceCount());

	for(index_t fid = inputModel.nextFaceIndex(0); fid != 0; fid = inputModel.nextFaceIndex(fid))
	{
		face_t currentFace = inputModel.getFace(fid);
		vertex_t vertices[3] = {
			inputModel.getVertex(currentFace.vertices[0]),
			inputModel.getVertex(currentFace.vertices[1]),
			inputModel.getVertex(currentFace.vertices[2])
		};

		vertex_t innerVertices[3];
		for(unsigned i=0; i<3; i++) {
			innerVertices[i].position = (vertices[i].position + vertices[(i+1)%3].position) / 2.0f;
			innerVertices[i].normal = (vertices[i].normal + vertices[(i+1)%3].normal) / 2.0f;
		}

		resultModel.addFace(vertices[0], innerVertices[0], vertices[1]);
		resultModel.addFace(vertices[1], innerVertices[1], vertices[2]);
		resultModel.addFace(vertices[2], innerVertices[2], vertices[0]);
		resultModel.addFace(innerVertices[0], innerVertices[1], innerVertices[2]);
	}

	dbg(" -> " << resultModel.faceCount() << " faces\n");
	return resultModel;
}

model loopSubdivOperator::operator()(const model& inputModel) {
	dbg("Loop subdiv\n");

	model resultModel;
	std::map<index_t, vertex_t>	innerVertices;
	std::map<index_t, vertex_t> modifiedVertices;

	for(index_t eid = inputModel.nextEdgeIndex(0); eid != 0; eid = inputModel.nextEdgeIndex(eid))
	{
		edge_t currentEdge = inputModel.getEdge(eid);
		indexSet_t facesWidthEdge = inputModel.findFacesWithEdge(eid);
		indexSet_t neighborEdgeIds = inputModel.extractEdgesFromFaces(facesWidthEdge);

		indexSet_t neighborVertexIds = inputModel.extractVerticesFromEdges(neighborEdgeIds);
			neighborVertexIds.erase(currentEdge.first);
			neighborVertexIds.erase(currentEdge.second);

		vertexSet_t currentVertices;
			currentVertices.insert(inputModel.getVertex(currentEdge.first));
			currentVertices.insert(inputModel.getVertex(currentEdge.second));

		vertexSet_t neighborVertices = inputModel.extractVerticesFromIndices(neighborVertexIds);

		vertex_t innerVertex;
			innerVertex.position = glm::vec3(0.0f);
			innerVertex.normal = glm::vec3(0.0f);

		for(const vertex_t& v : currentVertices) {
			innerVertex.position += 3.0f/8.0f * v.position;
			innerVertex.normal += 3.0f/8.0f * v.normal;
		}

		for(const vertex_t& v : neighborVertices) {
			innerVertex.position += 1.0f/8.0f * v.position;
			innerVertex.normal += 1.0f/8.0f * v.normal;
		}

		innerVertex.normal = glm::normalize(innerVertex.normal);

		innerVertices.insert({eid, innerVertex});
	}

	//

	for(index_t vid = inputModel.nextVertexIndex(0); vid != 0; vid = inputModel.nextVertexIndex(vid))
	{
		vertex_t currentVertex = inputModel.getVertex(vid);
		indexSet_t facesWithVertex = inputModel.findFacesWithVertex(vid);
		indexSet_t neighborVertexIds = inputModel.extractVerticesFromFaces(facesWithVertex);
			neighborVertexIds.erase(vid);

		vertexSet_t neighborVertices = inputModel.extractVerticesFromIndices(neighborVertexIds);

		vertex_t modifiedVertex;
			modifiedVertex.position = 5.0f/8.0f * currentVertex.position;
			modifiedVertex.normal = 5.0f/8.0f * currentVertex.normal;

		unsigned n = neighborVertices.size();

		for(const vertex_t& v : neighborVertices) {
			modifiedVertex.position += (3.0f/(8.0f * n)) * v.position;
			modifiedVertex.normal += (3.0f/(8.0f * n)) * v.normal;
		}

		modifiedVertex.normal = glm::normalize(modifiedVertex.normal);

		modifiedVertices.insert({vid, modifiedVertex});
	}

	//

	int i = 0;
	for(index_t fid = inputModel.nextFaceIndex(0); fid != 0; fid = inputModel.nextFaceIndex(fid))
	{
		face_t currentFace = inputModel.getFace(fid);

		indexSeq_t currentEdgeIds = {currentFace.edges[0], currentFace.edges[1], currentFace.edges[2]};
		indexSeq_t outerVertexIds;
		for(const index_t& eid : currentEdgeIds) 
			outerVertexIds.push_back(inputModel.getEdge(eid).first);

		vertexSeq_t outerVertices;
		vertexSeq_t currentInnerVertices;

		for(const index_t& ind : outerVertexIds)
			outerVertices.push_back(modifiedVertices[ind]);

		for(const index_t& ind : currentEdgeIds)
			currentInnerVertices.push_back(innerVertices[ind]);

		for(unsigned i=0; i<3; i++)
			resultModel.addFace(currentInnerVertices[i], outerVertices[(i+1)%3], currentInnerVertices[(i+1)%3]);

		resultModel.addFace(currentInnerVertices[0], currentInnerVertices[1], currentInnerVertices[2]);

		if(++i >= 6)
			break;
	}

	//

	return resultModel;
}
