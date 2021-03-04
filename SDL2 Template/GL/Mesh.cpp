#include "Mesh.h"

#include <tuple>

namespace gl {
	Camera& operator<<(Camera& target, const std::tuple<Mesh::Assembly, TypeCode, Size, Size>& source)
	{
		using std::get;
		glDrawElements(
			get<0>(source),	// geometry format
			get<3>(source),	// count of elements in block
			static_cast<GLenum>(get<1>(source)),	// code for type of element block
			reinterpret_cast<void*>(get<2>(source))	// offset from beginning of block
		);
		return target;
	}

	Camera& operator<<(Camera& target, const Mesh& source)
	{
		target << source.vertices; // prep vertex array
		for (auto& surface : source.surfaces) { // render each surface
			target << std::make_tuple(surface.mode, source.elementType, surface.start * TypeAlloc[source.elementType], surface.count);
		}
		return target << Vertex::Array{ nullptr }; // deactivate vertex array
	}

	Object::Object(Mesh*&& mesh, Program& prog)
		: Object{ std::shared_ptr<const Mesh>{mesh}, prog }
	{}

	Object::Object(std::shared_ptr<const Mesh> mesh, Program& program)
		: _mesh{ mesh }, _program{ program }
	{}
}
