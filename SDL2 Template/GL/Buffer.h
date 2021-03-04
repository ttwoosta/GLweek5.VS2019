#pragma once

#ifndef OPENGL_WRAPPER_BUFFER
#define OPENGL_WRAPPER_BUFFER

#include <cstddef>
#include <vector>
#include <memory>

#include "OpenGL.h"

namespace gl {
    class GeneralBuffer;
    
    template<> Name<GeneralBuffer>::~Name();
    
    class GeneralBuffer: public Name<GeneralBuffer> {
        struct unmapper {
            GLenum buffer;
            void operator() (void*) { glUnmapBuffer(buffer); }
        };
    public:
        enum Usage : GLenum {
            StreamDraw  = GL_STREAM_DRAW,
            StreamRead  = GL_STREAM_READ,
            StreamCopy  = GL_STREAM_COPY,
            StaticDraw  = GL_STATIC_DRAW,
            StaticRead  = GL_STATIC_READ,
            StaticCopy  = GL_STATIC_COPY,
            DynamicDraw = GL_DYNAMIC_DRAW,
            DynamicRead = GL_DYNAMIC_READ,
            DynamicCopy = GL_DYNAMIC_COPY,
        };
        enum Contents : GLenum {
            VertexArray     = GL_ARRAY_BUFFER, 
            ElementArray    = GL_ELEMENT_ARRAY_BUFFER, 
            UniformBlock    = GL_UNIFORM_BUFFER,
        };
        template <typename T> using mapped_ptr = std::unique_ptr<T, unmapper>;
    protected:
        GeneralBuffer() = default;
        GeneralBuffer(value_type n);
        constexpr GeneralBuffer(nullptr_t) : Name{ nullptr } {}

        void Activate(Contents buffer) const;
        
        static void Deactivate(Contents target);

		void Release();
        
        template <typename T>
        mapped_ptr<T> Access(Contents target)
        {
            return mapped_ptr<T> {static_cast<T*>(glMapBuffer(target, GL_READ_WRITE)), unmapper{target}};
        }
        
        template <typename T>
        mapped_ptr<const T> Access(Contents target) const
        {
            return mapped_ptr<const T>  {static_cast<const T*>(glMapBuffer(target, GL_READ_ONLY)), unmapper{target}};
        }
        
        template<typename E>
        GeneralBuffer& Load(Contents buffer, Usage role, const E* source, std::size_t count)
        {
            Activate(buffer);
            glBufferData(buffer, sizeof(E) * count, source, role);
            return *this;
        }
        
        template<typename E>
        GeneralBuffer& Load(Contents buffer, Usage role, const std::vector<E>& source)
        {
            return Load(buffer, role, source.data(), source.size());
        }

        GeneralBuffer& Reserve(Contents buffer, Usage role, std::size_t count)
        {
            Activate(buffer);
            glBufferData(buffer, count, nullptr, role);
            return *this;
        }
    };

    template <GeneralBuffer::Contents target>
    class Buffer: public GeneralBuffer {
    public:
        Buffer() = default;
        constexpr Buffer(nullptr_t) : GeneralBuffer(nullptr) {}
        void Activate() const { GeneralBuffer::Activate(target); }
        static void Deactivate() { GeneralBuffer::Deactivate(target); }
        
        template <typename T>
		mapped_ptr<T> Access() { Activate();  return GeneralBuffer::Access(target); }
        template <typename T>
		mapped_ptr<const T> Access() const { Activate();  return GeneralBuffer::Access(target); }

        using GeneralBuffer::Release;
        
        template <typename E>
        Buffer<target>& Load(Usage role, const E* source, std::size_t count)
        {
            GeneralBuffer::Load(target, role, source, count);
            return *this;
        }
        
        template <typename E>
        Buffer<target>& Load(Usage role, const std::vector<E>& source)
        {
            GeneralBuffer::Load(target, role, source);
            return *this;
        }

        Buffer<target>& Reserve(Usage role, std::size_t count)
        {
            GeneralBuffer::Reserve(target, role, count);
            return *this;
        }
    };
    
    template <>
    class Buffer<GeneralBuffer::UniformBlock>: public GeneralBuffer {
    public:
        using BindingPoint = Uint;
        enum struct ParameterName : GLenum {
            Binding = GL_UNIFORM_BLOCK_BINDING,
            DataSize = GL_UNIFORM_BLOCK_DATA_SIZE,
            NameLength = GL_UNIFORM_BLOCK_NAME_LENGTH,
            ActiveUniforms = GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
            ActiveUniformIndices = GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
        };
        
        Buffer() = default;
        constexpr Buffer(nullptr_t) : GeneralBuffer(nullptr) {}
        BindingPoint Activate(BindingPoint point = 0) const { glBindBufferBase(GL_UNIFORM_BUFFER, point, _name); return point; }
        static BindingPoint Deactivate(BindingPoint point = 0) { glBindBufferBase(GL_UNIFORM_BUFFER, point, 0); return point; }
        
        template <typename T>
		mapped_ptr<T> Access() { Activate(); return GeneralBuffer::Access<T>(UniformBlock); }
        template <typename T>
		mapped_ptr<const T> Access() const { Activate();  return GeneralBuffer::Access<T>(UniformBlock); }
        
        using GeneralBuffer::Release;
        
        template <typename E>
        Buffer<UniformBlock>& Load(Usage role, const E* source, std::size_t count)
        {
            GeneralBuffer::Load(UniformBlock, role, source, count);
            return *this;
        }
        
        template <typename E>
        Buffer<UniformBlock>& Load(Usage role, const std::vector<E>& source)
        {
            GeneralBuffer::Load(UniformBlock, role, source);
            return *this;
        }

        Buffer<UniformBlock>& Reserve(Usage role, std::size_t count)
        {
            GeneralBuffer::Reserve(UniformBlock, role, count);
            return *this;
        }
    };
    
    using ArrayBuffer = Buffer<GeneralBuffer::VertexArray>;
    using ElementArrayBuffer = Buffer<GeneralBuffer::ElementArray>;
    using UniformBuffer = Buffer<GeneralBuffer::UniformBlock>;
}

#endif
