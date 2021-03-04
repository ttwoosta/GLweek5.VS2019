#pragma once

#ifndef OPENGL_WRAPPER_MESH
#define OPENGL_WRAPPER_MESH

#include <string>
#include <vector>
#include <memory>

#include "OpenGL.h"
#include "Buffer.h"
#include "Vertex.h"
#include "Camera.h"

#include "glm/glm.hpp"

namespace gl {
	// packages vertex array with one or more element blocks
	class Mesh {
	public:
		enum Assembly : GLenum {
			Points = GL_POINTS, 
			LineStrip = GL_LINE_STRIP, 
			LineLoop = GL_LINE_LOOP, 
			Lines = GL_LINES, 
			TriangleStrip = GL_TRIANGLE_STRIP, 
			TriangleFan = GL_TRIANGLE_FAN, 
			Triangles = GL_TRIANGLES, 
			Patches = GL_PATCHES
		};
		struct SubMesh {
			Assembly mode;
			Size start;
			Size count;
		};
		Mesh() = default;
		friend Camera& operator<<(Camera& target, const Mesh&);

		// not defined locally! 
		// specific geometry packages will define their own installation processes
		template <typename T>
		Mesh& operator <<= (const T&);
	private:
		Vertex::Array vertices;
		TypeCode elementType;
		std::vector<SubMesh> surfaces;
	};

	// packages rendering specifics with a mesh that can be shared with other objects
	class Object {
	public:
		Object(Mesh*&& mesh, Program& program);
		Object(std::shared_ptr<const Mesh> mesh, Program& program);

		friend Camera& operator <<(Camera& output, const Object& data)
		{
			output << data._program; // activate program
			data._program.Uniform<Matrix4>("transform") = data.transform; // set transform
			output << *data._mesh; // render mesh contents
			return output;
		}

		Matrix4 transform;
	protected:
		std::shared_ptr<const Mesh> _mesh;
		Program& _program;
	};
}

#endif