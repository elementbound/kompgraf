#include <iostream>
#include <glm/gtx/io.hpp>
#include "model.h"

int main()
{
	model obj = loadModelFromOBJ("data/quad.obj");

	std::cout << "Vertices: \n";
	for(auto vid = obj.nextVertexIndex(0); vid != 0; vid = obj.nextVertexIndex(vid))
	{
		auto v = obj.getVertex(vid);

		std::cout << "\t" << vid << ": " << v.position << " " << v.normal << std::endl;
	}

	std::cout << "\nEdges: \n";
	for(auto eid = obj.nextEdgeIndex(0); eid != 0; eid = obj.nextEdgeIndex(eid))
	{
		auto e = obj.getEdge(eid);

		std::cout << "\t" << eid << ": " << e.first << " " << e.second << std::endl;
	}

	std::cout << "\nFaces: \n";
	for(auto fid = obj.nextFaceIndex(0); fid != 0; fid = obj.nextFaceIndex(fid))
	{
		auto f = obj.getFace(fid);

		std::cout << "\t" << fid << ": " << f.vertices[0] << " " << f.vertices[1] << " " << f.vertices[2] << std::endl;
	}

	return 0;
}