#include "OpenGL.h"

namespace gl {
	using size_type = std::underlying_type<TypeSize>::type;

	std::unordered_map<TypeCode, size_type> TypeAlloc = {
		{ TypeCode::Byte,	static_cast<size_type>(TypeSize::Byte) } ,
		{ TypeCode::Ubyte,	static_cast<size_type>(TypeSize::Ubyte) },
		{ TypeCode::Short,	static_cast<size_type>(TypeSize::Short) },
		{ TypeCode::Ushort,	static_cast<size_type>(TypeSize::Ushort) },
		{ TypeCode::Int,	static_cast<size_type>(TypeSize::Int) },
		{ TypeCode::Uint,	static_cast<size_type>(TypeSize::Uint) },
		{ TypeCode::Float,	static_cast<size_type>(TypeSize::Float) },
		{ TypeCode::Double,	static_cast<size_type>(TypeSize::Double) },
	};
}