#pragma once

#ifndef OPENGL_WRAPPER
#define OPENGL_WRAPPER

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <string>
#include <stdexcept>
#include <unordered_map>

#include "glm/glm.hpp"

namespace gl {
	using Bool = GLboolean;
	using Byte = GLbyte;
	using Ubyte = GLubyte;
	using Int = GLint;
	using Short = GLshort;
	using Ushort = GLushort;
	using Uint = GLuint;
	using Float = GLfloat;
	using Double = GLdouble;
	using Size = GLsizei;
	using Status = Int;

	template <class T>
	class Name {
    public:
		using value_type = Uint;

        explicit operator bool() const { return _name != 0; }
	protected:
        Name();
        ~Name();
        Name<T>& operator= (Name<T>&& source) & { _name = source._name; source._name = 0; return *this; }

		Name(value_type existing) : _name{ existing } {}
		constexpr Name(nullptr_t) noexcept : _name{ 0 } {}
        value_type _name;
	};

	using Color = glm::vec3;
	using ColorAlpha = glm::vec4;

	using Vector2 = glm::vec2;
	using Vector3 = glm::vec3;
	using Vector4 = glm::vec4;

	using Point3 = Vector3;

	using Matrix4 = glm::mat4;
	using Quaternion = glm::quat;

	enum struct TypeCode : GLenum {
		Byte = GL_BYTE,
		Ubyte = GL_UNSIGNED_BYTE,
		Short = GL_SHORT,
		Ushort = GL_UNSIGNED_SHORT,
		Int = GL_INT,
		Uint = GL_UNSIGNED_INT,
		Float = GL_FLOAT,
		Double = GL_DOUBLE,
	};

	template <typename T>
	constexpr TypeCode TypeSignal = -1;

	template <>
	constexpr TypeCode TypeSignal<Byte> = TypeCode::Byte;

	template <>
	constexpr TypeCode TypeSignal<Ubyte> = TypeCode::Ubyte;

	template <>
	constexpr TypeCode TypeSignal<Short> = TypeCode::Short;

	template <>
	constexpr TypeCode TypeSignal<Ushort> = TypeCode::Ushort;

	template <>
	constexpr TypeCode TypeSignal<Int> = TypeCode::Int;

	template <>
	constexpr TypeCode TypeSignal<Uint> = TypeCode::Uint;

	template <>
	constexpr TypeCode TypeSignal<Float> = TypeCode::Float;

	template <>
	constexpr TypeCode TypeSignal<Double> = TypeCode::Double;

	enum struct TypeSize : Size {
		Byte = sizeof(gl::Byte),
		Ubyte = sizeof(gl::Ubyte),
		Short = sizeof(gl::Short),
		Ushort = sizeof(gl::Ushort),
		Int = sizeof(gl::Int),
		Uint = sizeof(gl::Uint),
		Float = sizeof(gl::Float),
		Double = sizeof(gl::Double),
	};

	extern std::unordered_map<TypeCode, Size> TypeAlloc;
}

#define TRAPGL(a) { auto e = glGetError(); if (e) throw std::runtime_error(std::string(a) + std::to_string(e)); }

#endif