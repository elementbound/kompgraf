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

model subdivOperator::apply(const model& inputModel) {
	return this->operator()(inputModel);
}

model subdivOperator::operator()(const model& inputModel) {
	model resultModel = inputModel;
	return resultModel;
}

model sierpinskiSubdivOperator::operator()(const model& inputModel) {
	dbg("Sierpinski subdiv\n");
	model resultModel;

	unsigned processedFaceCount = 0;
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

		resultModel.addFace(innerVertices[0], innerVertices[1], vertices[1]);
		resultModel.addFace(innerVertices[1], innerVertices[2], vertices[2]);
		resultModel.addFace(innerVertices[2], innerVertices[0], vertices[0]);

		processedFaceCount++;
		rtdbg("Progress: " << (processedFaceCount*100 / inputModel.faceCount()) << '%', 0.05);
	}

	dbg("Midpoint subdiv, " << inputModel.faceCount() << " -> " << resultModel.faceCount() << " faces\n");
	return resultModel;
}

model loopSubdivOperator::operator()(const model& inputModel) {
	dbg("Loop subdiv\n");

	model resultModel;
	std::map<index_t, vertex_t>	innerVertices;
	std::map<index_t, vertex_t> modifiedVertices;

	unsigned progressCounter = 0;
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

		float weightSum = 0.0f;
		for(const vertex_t& v : currentVertices) {
			innerVertex.position += 3.0f/8.0f * v.position;
			innerVertex.normal += 3.0f/8.0f * v.normal;

			weightSum += 3.0f/8.0f;
		}

		for(const vertex_t& v : neighborVertices) {
			innerVertex.position += 1.0f/8.0f * v.position;
			innerVertex.normal += 1.0f/8.0f * v.normal;

			weightSum += 1.0f/8.0f;
		}

		innerVertex.position /= weightSum;
		innerVertex.normal /= weightSum;

		innerVertex.normal = glm::normalize(innerVertex.normal);

		innerVertices.insert({eid, innerVertex});

		progressCounter++;
		rtdbg("Calculating inner vertices... " << 100*progressCounter/inputModel.edgeCount() << '%', 0.05);
	}
	dbg("Calculating inner vertices... Done\n");

	//

	progressCounter = 0;
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

		progressCounter++;
		rtdbg("Modifying original vertices... " << 100*progressCounter/inputModel.vertexCount() << '%', 0.05);
	}
	dbg("Modifying original vertices... Done\n");

	//

	progressCounter = 0;
	for(index_t fid = inputModel.nextFaceIndex(0); fid != 0; fid = inputModel.nextFaceIndex(fid))
	{
		face_t currentFace = inputModel.getFace(fid);

		indexSeq_t currentEdgeIds = {currentFace.edges[0], currentFace.edges[1], currentFace.edges[2]};
		indexSeq_t outerVertexIds = {currentFace.vertices[0], currentFace.vertices[1], currentFace.vertices[2]};

		vertexSeq_t outerVertices;
		vertexSeq_t currentInnerVertices;

		for(const index_t& ind : outerVertexIds)
			outerVertices.push_back(modifiedVertices[ind]);

		for(const index_t& ind : currentEdgeIds)
			currentInnerVertices.push_back(innerVertices[ind]);

		for(unsigned i=0; i<3; i++)
			resultModel.addFace(currentInnerVertices[i], outerVertices[(i+1)%3], currentInnerVertices[(i+1)%3], false);

		resultModel.addFace(currentInnerVertices[0], currentInnerVertices[1], currentInnerVertices[2], false);

		progressCounter++;
		rtdbg("Assembling new mesh... " << 100*progressCounter/inputModel.faceCount() << '%', 0.05);
	}
	dbg("Assembling new mesh... Done\n");

	resultModel.removeDuplicateVertices();
	resultModel.removeDuplicateEdges();

	//

	dbg("\t" << inputModel.faceCount() << " -> " << resultModel.faceCount() << '\n');

	return resultModel;
}

//Small util function for butterflySubdiv
template <typename T>
	void set_subtract(std::set<T>& from, std::set<T>& sub) {
		for(const T& item : sub)
			from.erase(item);
	}

model butterflySubdivOperator::operator()(const model& inputModel) {
	dbg("Butterfly subdiv\n");

	model resultModel;
	std::map<index_t, vertex_t> innerVertices;

	unsigned progressCounter = 0;
	for(index_t edgeId = inputModel.nextEdgeIndex(0); edgeId != 0; edgeId = inputModel.nextEdgeIndex(edgeId)) {
		const edge_t& currentEdge = inputModel.getEdge(edgeId);
		vertex_t innerVertex;

		indexSet_t neighborsByLevel[3];
		//Level 0 "neighbors"
		neighborsByLevel[0] = {currentEdge.first, currentEdge.second};

		//Level 1 neighbors
		indexSet_t neighborFaceIds = inputModel.findFacesWithEdge(edgeId);
		indexSet_t neighborEdgeIds = inputModel.extractEdgesFromFaces(neighborFaceIds);
		neighborsByLevel[1] = inputModel.extractVerticesFromFaces(neighborFaceIds);

		//Level 2 neighbors
		for(index_t eid : neighborEdgeIds) 
			for(index_t fid : inputModel.findFacesWithEdge(eid))
				neighborFaceIds.insert(fid);

		neighborsByLevel[2] = inputModel.extractVerticesFromFaces(neighborFaceIds);

		//Remove duplicates from sets
		set_subtract(neighborsByLevel[1], neighborsByLevel[0]);
		set_subtract(neighborsByLevel[2], neighborsByLevel[0]);
		set_subtract(neighborsByLevel[2], neighborsByLevel[1]);

		//Apply them with weights
		//Also, try handling wrong topology somewhat gracefully
		float weightSum = 0.0f;

		innerVertex.position = glm::vec3(0.0f);
		innerVertex.normal = glm::vec3(0.0f);

		for(index_t vid : neighborsByLevel[0]) {
			weightSum += 0.5f;

			const vertex_t& currentVertex = inputModel.getVertex(vid);
			innerVertex.position += 0.5f * currentVertex.position;
			innerVertex.normal += 0.5f * currentVertex.normal;
		}

		for(index_t vid : neighborsByLevel[1]) {
			weightSum += 0.125f;

			const vertex_t& currentVertex = inputModel.getVertex(vid);
			innerVertex.position += 0.125f * currentVertex.position;
			innerVertex.normal += 0.125f * currentVertex.normal;
		}

		for(index_t vid : neighborsByLevel[2]) {
			weightSum += -0.0625f;

			const vertex_t& currentVertex = inputModel.getVertex(vid);
			innerVertex.position += -0.0625f * currentVertex.position;
			innerVertex.normal += -0.0625f * currentVertex.normal;
		}

		innerVertex.position /= weightSum;
		innerVertex.normal /= weightSum;
		innerVertex.normal = glm::normalize(innerVertex.normal);

		//Store
		innerVertices.insert({edgeId, innerVertex});

		//Progress
		progressCounter++;
		rtdbg("\tCalculating inner vertices... " << 100*progressCounter / inputModel.edgeCount() << "%",0.05);
	}
	dbg("\tCalculating inner vertices... Done\n");

	progressCounter = 0;
	for(index_t fid = inputModel.nextFaceIndex(0); fid != 0; fid = inputModel.nextFaceIndex(fid))
	{
		face_t currentFace = inputModel.getFace(fid);

		indexSeq_t currentEdgeIds = {currentFace.edges[0], currentFace.edges[1], currentFace.edges[2]};
		indexSeq_t outerVertexIds = {currentFace.vertices[0], currentFace.vertices[1], currentFace.vertices[2]};

		vertexSeq_t outerVertices;
		vertexSeq_t currentInnerVertices;

		for(const index_t& ind : outerVertexIds)
			outerVertices.push_back(inputModel.getVertex(ind));

		for(const index_t& ind : currentEdgeIds)
			currentInnerVertices.push_back(innerVertices[ind]);

		for(unsigned i=0; i<3; i++)
			resultModel.addFace(currentInnerVertices[i], outerVertices[(i+1)%3], currentInnerVertices[(i+1)%3], false);

		resultModel.addFace(currentInnerVertices[0], currentInnerVertices[1], currentInnerVertices[2], false);

		progressCounter++;
		rtdbg("\tAssembling new mesh... " << 100*progressCounter/inputModel.faceCount() << '%', 0.05);
	}
	dbg("\tAssembling new mesh... Done\n");

	resultModel.removeDuplicateVertices();
	resultModel.removeDuplicateEdges();

	//

	dbg("\t" << inputModel.faceCount() << " -> " << resultModel.faceCount() << '\n');

	return resultModel;
}