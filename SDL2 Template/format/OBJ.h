#pragma once

#ifndef FORMAT_GEOMETRY_OBJ
#define FORMAT_GEOMETRY_OBJ

#include <string>
#include <vector>
#include <istream>

#include "GL/OpenGL.h"

namespace geometry {
	using namespace gl;
	class OBJ {
	public:
		struct Vertex {
			Point3 position;
			Vector3 normal;
			Vector2 uv;
		};

		using vertex_type = Vertex;
		using index_type = Ushort;
		using count_type = Ushort;

		OBJ(const std::string& filename);

		std::vector<vertex_type> vertices;
		std::vector<index_type> faces;
		std::vector<count_type> ranges;
	private:
		struct Index {
			Short p, t, n;
			Index(const std::string& encoding, Short pCount, Short tCount, Short nCount);

			bool operator==(const Index& other) const
			{
				return ((p == other.p) || (p < 0 && other.p < 0))
					&& ((t == other.t) || (t < 0 && other.t < 0))
					&& ((n == other.n) || (n < 0 && other.n < 0));
			}
		private:
			Short Enter(std::istream& source, Short limit)
			{
				Short result;
				if (source.peek() != '/' && source >> result) {
					result = result < 0 ? limit - result : --result;
				}
				else {
					result = -1;
				}
				source.ignore();
				return result;
			}
		};

		class hash : public std::hash<std::string> {
		public:
			size_t operator() (Index const i) const {
				return std::hash<std::string>::operator () (std::string(reinterpret_cast<char const*>(&i), sizeof(Index)));
			}
		};
	};

}

#endif
