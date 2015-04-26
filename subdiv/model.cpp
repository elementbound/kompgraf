#include "model.h"
#include "glwrap/util.h" //buffer << glm::vec3

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

//Use a negative value to use default tolerance
index_t model::findVertex(vertex_t v, float posTolerance, float normalTolerance) {
	if(posTolerance < 0.0f ) posTolerance = defaultPositionTolerance;
	if(normalTolerance < 0.0f) normalTolerance = defaultNormalTolerance;

	for(const std::pair<index_t, vertex_t>& p : m_Vertices) {

		if(glm::length(v.position - p.second.position) < posTolerance && 
			glm::abs(glm::dot(v.normal, p.second.normal)) > 1.0f-normalTolerance )
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

void model::clear() {
	m_Vertices.clear();
	m_Edges.clear();
	m_Faces.clear();
}

void model::build_drawable(basic_mesh& resultMesh) const {
	resultMesh.clear_streams();

	unsigned pos = resultMesh.add_stream();
	unsigned nor = resultMesh.add_stream();

	resultMesh[pos].type = GL_FLOAT;
	resultMesh[pos].buffer_type = GL_ARRAY_BUFFER;
	resultMesh[pos].components = 3;
	resultMesh[pos].normalized = 0;
	resultMesh[pos].name = "vertexPosition";
	
	resultMesh[nor].type = GL_FLOAT;
	resultMesh[nor].buffer_type = GL_ARRAY_BUFFER;
	resultMesh[nor].components = 3;
	resultMesh[nor].normalized = 0;
	resultMesh[nor].name = "vertexNormal";

	for(const auto& p : m_Faces)
	{
		const auto& f = p.second;

		for(unsigned i=0; i<3; i++)
		{
			const vertex_t& v = getVertex(f.vertices[i]);

			resultMesh[pos].data << v.position;
			resultMesh[nor].data << v.normal;
		}
	}

	resultMesh.draw_mode = GL_TRIANGLES;
	resultMesh.storage_policy = GL_STATIC_DRAW;
	resultMesh.upload();
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

	//Assembly
	std::vector<index_t> face_buffer;
	face_buffer.reserve(3);

	for(const auto& f: obj_vertices) {
		const index_t pos_index = f[0] - 1;
		const index_t uv_index = f[1] - 1;
		const index_t normal_index = f[2] - 1;

		vertex_t v;
			v.position = obj_positions[pos_index];
			v.normal = obj_normals[normal_index];

		index_t vid = retModel.addVertex(v, true);

		face_buffer.push_back(vid);
		if(face_buffer.size() == 3)
		{
			retModel.addFace(face_buffer[0], face_buffer[1], face_buffer[2]);
			face_buffer.clear();
		}
	}

	return retModel;
}

model loadModelFromOBJ(const char* fname)
{
	std::ifstream is(fname);
	return loadModelFromOBJ(is);
}