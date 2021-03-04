#include "OBJ.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GL/Mesh.h"
using namespace std;

namespace geometry {

	OBJ::Index::Index(const string& encoding, Short pCount, Short tCount, Short nCount)
	{
		istringstream description(encoding);

		p = Enter(description, pCount);
		t = Enter(description, tCount);
		n = Enter(description, nCount);
	}

	OBJ::OBJ(const std::string& filename)
	{
		string line;
		vector<Point3> vList;
		vector<Vector3> vnList;
		vector<Vector2> vtList;
		unordered_map<Index, Short, OBJ::hash> verts;
		for (ifstream source(filename); getline(source, line); ) {
			istringstream input(line);
			string command;
			input >> command;
			if (command == "v") {
				Point3 v;
				input >> v.x >> v.y >> v.z;
				vList.push_back(v);
			}
			else if (command == "vt") {
				Vector2 t;
				input >> t.x >> t.y;
				vtList.push_back(t);
			}
			else if (command == "vn") {
				Vector3 n;
				input >> n.x >> n.y >> n.z;
				vnList.push_back(n);
			}
			else if (command == "f") {
				string corner;
				int corners = 0;
				while (input >> corner) {
					Index id(corner, vList.size(), vtList.size(), vnList.size());
					if (!verts.count(id)) {
						vertex_type v;
						if (id.p >= 0) v.position = vList[id.p];
						if (id.t >= 0) v.uv = vtList[id.t];
						if (id.n >= 0) v.normal = vnList[id.n];
						verts[id] = vertices.size();
						vertices.push_back(v);
					}
					corners++;
					faces.push_back(verts[id]);
				}
				ranges.push_back(corners);
			}
		}
	}
}

namespace gl {
	template<>
	Mesh& Mesh::operator<<= <geometry::OBJ>(const geometry::OBJ& source)
	{
		using geometry::OBJ;
		ArrayBuffer verts;
		ElementArrayBuffer faces;

		elementType = TypeSignal<OBJ::index_type>;

		verts.Load(verts.StaticDraw, source.vertices);
		faces.Load(faces.StaticDraw, source.faces);
		vertices
			<< Vertex::ArrayAttribute<Float[3], OBJ::Vertex>{0, verts}
			<< Vertex::ArrayAttribute<Float[3], OBJ::Vertex>{1, sizeof(OBJ::Vertex::position), verts}
			<< Vertex::ArrayAttribute<Float[2], OBJ::Vertex>{2, sizeof(OBJ::Vertex::position) + sizeof(OBJ::Vertex::normal), verts}
			<< faces
			<< Deactivate;

		OBJ::index_type offset = 0;
		for (OBJ::index_type face : source.ranges) {
			surfaces.push_back({ Mesh::TriangleFan, offset, face });
			offset += face;
		}

		return *this;
	}
}