#include "model.h"
#include "glwrap/util.h" //buffer << glm::vec3
#include <array>
#include "debug.h"

typedef model::vertex_t 	vertex_t;
typedef model::edge_t 		edge_t;
typedef model::face_t 		face_t;

typedef model::index_t 		index_t;
typedef model::indexSet_t 	indexSet_t;

typedef model::vertexSet_t	vertexSet_t;
typedef model::edgeSet_t	edgeSet_t;
typedef model::faceSet_t	faceSet_t;

bool operator<(const glm::vec3& a, const glm::vec3& b)
{
	if(a.x != b.x)
		return a.x < b.x;
	if(a.y != b.y)
		return a.y < b.y;
	if(a.z != b.z)
		return a.z < b.z;

	return 0;
}

bool vertex_t::operator<(const vertex_t& rhs) const {
	if(position != rhs.position)
		return position < rhs.position;
	else if(normal != rhs.normal)
		return normal < rhs.normal;
	return 0;
}

bool face_t::operator<(const face_t& rhs) const {
	if(vertices != rhs.vertices)
		return vertices < rhs.vertices;
	else if(edges != rhs.edges)
		return edges < rhs.edges;
	return 0;
}

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

bool model::areVerticesEqual(const vertex_t& v1, const vertex_t& v2, float posTolerance, float normalTolerance) const {
return	glm::length(v1.position - v2.position) < posTolerance && 
		glm::abs(glm::dot(v1.normal, v2.normal)) > 1.0f-normalTolerance;
}

bool model::areVerticesEqual(index_t v1, index_t v2, float posTolerance, float normalTolerance) const {
	return areVerticesEqual(getVertex(v1), getVertex(v2), posTolerance, normalTolerance);
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
	bool addEdge = true;

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

index_t model::addFace(vertex_t v1, vertex_t v2, vertex_t v3, bool checkForDuplicates) {
	index_t retId = genFaceIndex();
	face_t f;
	f.vertices = {	addVertex(v1, checkForDuplicates), 
					addVertex(v2, checkForDuplicates), 
					addVertex(v3, checkForDuplicates) };

	f.edges = {	addEdge(f.vertices[0], f.vertices[1], checkForDuplicates), 
				addEdge(f.vertices[1], f.vertices[2], checkForDuplicates), 
				addEdge(f.vertices[2], f.vertices[0], checkForDuplicates) };

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

//Use a negative value to use default tolerance
index_t model::findVertex(vertex_t v, float posTolerance, float normalTolerance) {
	if(posTolerance < 0.0f ) posTolerance = defaultPositionTolerance;
	if(normalTolerance < 0.0f) normalTolerance = defaultNormalTolerance;

	for(const std::pair<index_t, vertex_t>& p : m_Vertices) {

		if(areVerticesEqual(v, p.second, posTolerance, normalTolerance))
			return p.first;
	}

	return 0;
}

index_t model::findEdge(index_t v1, index_t v2, bool orderMatters) {
	edge_t needle = edge_t(v1, v2);
	edge_t rev_needle = edge_t(v2, v1);

	for(const std::pair<index_t, edge_t>& p : m_Edges)
	{
		if(needle == p.second)
			return p.first;

		if(!orderMatters && rev_needle == p.second)
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

indexSet_t model::findFacesWithVertex(index_t v) const {
	indexSet_t retSet;

	for(const auto& p : m_Faces)
	{
		const index_t& index = p.first;
		const face_t& face = p.second;

		if(face.vertices[0] == v || face.vertices[1] == v || face.vertices[2] == v)
			retSet.insert(index);
	}

	return retSet;
}

//

void model::removeDuplicateVertices(float posTolerance, float normalTolerance)
{
	typedef std::array<int, 3> chunk_t;

	if(posTolerance < 0.0f) posTolerance = defaultPositionTolerance;
	if(normalTolerance < 0.0f) normalTolerance = defaultNormalTolerance;

	dbg("Removing duplicate vertices\n");
	dbg("\tPosition tolerance: " << posTolerance << '\n');
	dbg("\tNormal tolerance: " << normalTolerance << '\n');


	indexSet_t verticesToRemove;
	std::map<index_t, index_t> vertexRemap;
	std::map<chunk_t, indexSet_t> vertexClusters;
	auto bounds = getBounds();
	int clusterResolution = 32;

	unsigned progressCounter = 0;
	for(const auto& p : m_Vertices)
	{
		const vertex_t& currentVertex = p.second;
		glm::vec3 meshSpacePos = (currentVertex.position - bounds.first) / (bounds.second - bounds.first);
		chunk_t currentChunk = {
			int(meshSpacePos.x * clusterResolution), 
			int(meshSpacePos.y * clusterResolution), 
			int(meshSpacePos.z * clusterResolution) 
		};

		vertexClusters[currentChunk].insert(p.first);

		progressCounter++;
		rtdbg("\tSorting vertices into chunks... " << 100*progressCounter / m_Vertices.size() << "%", 0.05);
	}
	dbg("\tSorting vertices into chunks... Done\n");

	progressCounter = 0;
	for(const auto& p : m_Vertices)
	{
		const vertex_t& currentVertex = p.second;
		glm::vec3 meshSpacePos = (currentVertex.position - bounds.first) / (bounds.second - bounds.first);
		chunk_t currentChunk = {
			int(meshSpacePos.x * clusterResolution), 
			int(meshSpacePos.y * clusterResolution), 
			int(meshSpacePos.z * clusterResolution) 
		};

		index_t remapId;

		for(const auto& c : vertexClusters[currentChunk]) {
			if(areVerticesEqual(p.first, c, posTolerance, normalTolerance)) {
				remapId = c;
				break;
			}
		}

		if(remapId != p.first)
			verticesToRemove.insert(p.first);

		vertexRemap.insert({p.first, remapId});

		progressCounter++;
		rtdbg("\tLooking for duplicates... " << 100*progressCounter / m_Vertices.size() << "%", 0.05);
	}
	dbg("\tLooking for duplicates... Done\n");

	/*dbg("\tVertex ID remaps: \n");
	for(auto& p : vertexRemap)
		dbg("\t\t" << p.first << " -> " << p.second << '\n');*/

	dbg("\tRemapping vertex indices\n");
	progressCounter = 0;
	for(auto& p : m_Edges)
	{
		edge_t& currentEdge = p.second;

		currentEdge.first = vertexRemap[currentEdge.first];
		currentEdge.second = vertexRemap[currentEdge.second];

		progressCounter++;
		rtdbg("\t\tEdges: " << 100*progressCounter / m_Edges.size() << "%", 0.05);
	}
	dbg("\t\tEdges: Done\n");

	progressCounter = 0;
	for(auto& p : m_Faces) 
	{
		face_t& currentFace = p.second;

		currentFace.vertices[0] = vertexRemap[currentFace.vertices[0]];
		currentFace.vertices[1] = vertexRemap[currentFace.vertices[1]];
		currentFace.vertices[2] = vertexRemap[currentFace.vertices[2]];

		progressCounter++;
		rtdbg("\t\tFaces: " << 100*progressCounter / m_Faces.size() << "%", 0.05);
	}
	dbg("\t\tFaces: Done\n");

	dbg("\tRemoving " << verticesToRemove.size() << " duplicate vertices\n");
	for(index_t vid : verticesToRemove)
		m_Vertices.erase(vid);

	dbg("\tDone!\n");
}

void model::removeDuplicateEdges(bool orderMatters) 
{
	dbg("Removing duplicate edges\n");

	indexSet_t edgesToRemove;
	std::map<index_t, index_t> edgeRemap;
	std::map<index_t, indexSet_t> firstVertexClusters; 

	unsigned progressCounter = 0;
	if(!orderMatters) {
		for(auto& p : m_Edges) {
			edge_t& currentEdge = p.second;

			if(currentEdge.first >= currentEdge.second)
				std::swap(currentEdge.first, currentEdge.second);

			progressCounter++;
			rtdbg("\tSorting edge vertices... " << 100*progressCounter / m_Edges.size() << "%", 0.05);
		}
		dbg("\tSorting edge vertices... Done\n");
	}

	progressCounter = 0;
	for(const auto& p : m_Edges) {
		const index_t& currentId = p.first;
		const edge_t& currentEdge = p.second;
		firstVertexClusters[currentEdge.first].insert(currentId);

		progressCounter++;
		rtdbg("\tClustering edges by first vertex... " << 100*progressCounter / m_Edges.size() << "%", 0.05);
	}
	dbg("\tClustering edges by first vertex... Done\n");

	progressCounter = 0;
	for(const auto& p : m_Edges) {
		const index_t& currentId = p.first;
		const edge_t& currentEdge = p.second;
		index_t remapId;

		for(const index_t& checkId : firstVertexClusters[currentEdge.first]) {
			if(getEdge(checkId) == currentEdge) {
				remapId = checkId;
				break;
			}
		}

		if(remapId != currentId) 
			edgesToRemove.insert(currentId);
		edgeRemap.insert({currentId, remapId});

		progressCounter++;
		rtdbg("\tLooking for duplicate edges... " << 100*progressCounter / m_Edges.size() << "%", 0.05);
	}
	dbg("\tLooking for duplicate edges... Done!\n");

	progressCounter = 0;
	for(auto& p : m_Faces)
	{
		face_t& currentFace = p.second;

		currentFace.edges[0] = edgeRemap[currentFace.edges[0]];
		currentFace.edges[1] = edgeRemap[currentFace.edges[1]];
		currentFace.edges[2] = edgeRemap[currentFace.edges[2]];

		progressCounter++;
		rtdbg("\tRemapping faces... " << 100*progressCounter / m_Faces.size() << "%", 0.05);
	}
	dbg("\tRemapping faces... Done\n");

	dbg("\tRemoving " << edgesToRemove.size() << " duplicates\n");
	for(index_t e : edgesToRemove)
		m_Edges.erase(e);

	dbg("\tDone!\n");
}

//

std::pair<glm::vec3, glm::vec3> model::getBounds() const {
	std::pair<glm::vec3, glm::vec3> bounds;

	if(!m_Vertices.empty()) {
		bounds.first  = m_Vertices.begin()->second.position;
		bounds.second = m_Vertices.begin()->second.position;
	}

	for(const auto& p : m_Vertices) {
		bounds.first  = glm::min(bounds.first,  p.second.position);
		bounds.second = glm::max(bounds.second, p.second.position);
	}

	return bounds;
}

//

indexSet_t model::extractVerticesFromEdges(const indexSet_t& inputSet) const {
	indexSet_t resultSet;

	for(const index_t& ind : inputSet)
	{
		edge_t item = getEdge(ind);
		resultSet.insert(item.first);
		resultSet.insert(item.second);
	}

	return resultSet;
}

indexSet_t model::extractVerticesFromFaces(const indexSet_t& inputSet) const {
	indexSet_t resultSet;

	for(const index_t& ind : inputSet)
	{
		face_t item = getFace(ind);

		resultSet.insert(item.vertices[0]);
		resultSet.insert(item.vertices[1]);
		resultSet.insert(item.vertices[2]);
	}

	return resultSet;
}

indexSet_t model::extractEdgesFromFaces(const indexSet_t& inputSet) const {
	indexSet_t resultSet;

	for(const index_t& ind : inputSet)
	{
		face_t item = getFace(ind);

		resultSet.insert(item.edges[0]);
		resultSet.insert(item.edges[1]);
		resultSet.insert(item.edges[2]);
	}

	return resultSet;
}

//

vertexSet_t model::extractVerticesFromIndices(const indexSet_t& inputSet) const {
	vertexSet_t resultSet;

	for(const index_t& ind : inputSet)
		resultSet.insert(getVertex(ind));

	return resultSet;
}

edgeSet_t	model::extractEdgesFromIndices(const indexSet_t& inputSet) const {
	edgeSet_t resultSet;

	for(const index_t& ind : inputSet)
		resultSet.insert(getEdge(ind));

	return resultSet;
}

faceSet_t	model::extractFacesFromIndices(const indexSet_t& inputSet) const {
	faceSet_t resultSet;

	for(const index_t& ind : inputSet)
		resultSet.insert(getFace(ind));

	return resultSet;
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

//

unsigned model::vertexCount() const {
	return m_Vertices.size();
}

unsigned model::edgeCount() const {
	return m_Edges.size();
}

unsigned model::faceCount() const {
	return m_Faces.size();
}


//

void model::clear() {
	m_Vertices.clear();
	m_Edges.clear();
	m_Faces.clear();
}

void model::build_drawable(basic_mesh& resultMesh) const {
	dbg("Building mesh\n");
	resultMesh.clear_streams();

	unsigned pos = resultMesh.add_stream();
	unsigned nor = resultMesh.add_stream();

	resultMesh[pos].type = GL_FLOAT;
	resultMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	resultMesh[pos].components = 3;
	resultMesh[pos].normalized = 0;
	resultMesh[pos].name = "vertexPosition";
	resultMesh[pos].data.capacity(m_Faces.size() * 3 * sizeof(float) * 3);
	
	resultMesh[nor].type = GL_FLOAT;
	resultMesh[nor].buffer_type = GL_ARRAY_BUFFER;
	resultMesh[nor].components = 3;
	resultMesh[nor].normalized = 0;
	resultMesh[nor].name = "vertexNormal";
	resultMesh[nor].data.capacity(m_Faces.size() * 3 * sizeof(float) * 3);

	unsigned face_count = 0;
	for(const auto& p : m_Faces)
	{
		const auto& f = p.second;

		for(unsigned i=0; i<3; i++)
		{
			const vertex_t& v = getVertex(f.vertices[i]);

			resultMesh[pos].data << v.position;
			resultMesh[nor].data << v.normal;
		}

		face_count++;
		rtdbg("\tBuffering faces... " << 100*face_count / m_Faces.size() << "%", 0.05);
	}
	dbg("\tBuffering faces... Done\n");

	resultMesh.draw_mode = GL_TRIANGLES;
	resultMesh.storage_policy = GL_STATIC_DRAW;
	dbg("\tUploading to GPU... ");
	resultMesh.upload();
	dbg("Done\n");
}

//

#include <sstream>
#include <string>
//strtod because std::stod is broken on my compiler ( mingw 4.8.1 )
#include <cstdlib>
#include <fstream>

#include <vector>

void read_vec2(std::istream& is, glm::vec2& v)
{
	std::string component[2];
	is >> component[0] >> component[1];
	
	v.x = (float)atof(component[0].c_str());
	v.y = (float)atof(component[1].c_str());
}

void read_vec3(std::istream& is, glm::vec3& v)
{
	std::string component[3];
	is >> component[0] >> component[1] >> component[2];
	
	v.x = (float)atof(component[0].c_str());
	v.y = (float)atof(component[1].c_str());
	v.z = (float)atof(component[2].c_str());
}

model loadModelFromOBJ(std::istream& is)
{
	model retModel;

	std::string line;
	std::stringstream parser;
	std::string token; 

	std::vector<glm::vec3> obj_positions;
	std::vector<glm::vec3> obj_normals;
	std::vector<glm::vec2> obj_texcoords;
	std::vector<std::array<index_t, 3>> obj_vertices;
	
	//
	
	obj_positions.reserve(8192);
	obj_normals.reserve(8192);
	obj_texcoords.reserve(8192);

	//Collect data
	dbg("[OBJ]Parsing data... ");
	while(is)
	{
		std::getline(is, line);
		
		//Ignore empty lines
		if(line.size() == 0)
			continue;
		
		//Ignore comments
		if(line[0] == '#')
			continue;
		
		parser.str(line);
		parser.seekg(0);
		
		parser >> token;

		if(token == "v")
		{
			glm::vec3 v;
			read_vec3(parser, v);
			
			obj_positions.push_back(v);
		} 
		else if(token == "vn")
		{
			glm::vec3 n;
			read_vec3(parser, n);
			
			obj_normals.push_back(n);
		}
		else if(token == "vt")
		{
			glm::vec2 uv;
			read_vec2(parser, uv);
			
			obj_texcoords.push_back(uv);
		}
		else if(token == "f")
		{
			std::array<index_t, 3> f;
			
			for(unsigned j=0; j<3; j++)
			{
				for(unsigned i=0; i<3; i++)
				{
					parser >> f[i];
					
					if(i!=2)
						parser.ignore(std::numeric_limits<std::streamsize>::max(), '/');
				}
				
				obj_vertices.push_back(f);
			}
		}
	}
	dbg("Done!\n");

	//Assembly
	std::vector<index_t> face_buffer;
	face_buffer.reserve(3);

	unsigned progressCounter = 0;
	for(const auto& f: obj_vertices) {
		const index_t pos_index = f[0] - 1;
		const index_t uv_index = f[1] - 1;
		const index_t normal_index = f[2] - 1;

		vertex_t v;
			v.position = obj_positions[pos_index];
			v.normal = obj_normals[normal_index];

		index_t vid = retModel.addVertex(v, false);

		face_buffer.push_back(vid);
		if(face_buffer.size() == 3)
		{
			retModel.addFace(face_buffer[0], face_buffer[1], face_buffer[2]);
			face_buffer.clear();
		}

		progressCounter++;
		rtdbg("[OBJ]Assembling model... " << (progressCounter*100 / obj_vertices.size()) << '%', 0.05);
	}
	dbg("[OBJ]Assembling model... Done!\n");

	retModel.removeDuplicateVertices();
	retModel.removeDuplicateEdges();

	return retModel;
}

model loadModelFromOBJ(const char* fname)
{
	std::ifstream is(fname);
	return loadModelFromOBJ(is);
}