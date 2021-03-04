#pragma once

#ifndef OPENGL_WRAPPER_SHADER
#define OPENGL_WRAPPER_SHADER

#include <string>
#include <list>
#include <unordered_map>

#include "OpenGL.h"
#include "Buffer.h"

namespace shader {
    extern std::string vFlat;
    extern std::string fFlat;
}

namespace gl {
    class Shader: public Name<Shader> {
    public:
        enum Kind : GLenum {
            Vertex = GL_VERTEX_SHADER,
            Fragment = GL_FRAGMENT_SHADER,
        };
        Shader(Kind kind, const std::string& source);
    protected:
        friend class Program;
    };
    
    class Program: public Name<Program> {
        template <typename T>
        struct BindingReference {
        public:
            BindingReference(T i, const Program& prog) : index{ i }, program{ prog } {}
            const T index;
            const Program& program;
        };
    public:
        Program(const Shader& vertex, const Shader& fragment);
        
        //template<typename C>
        //Program(const C& sh)
        //{
        //    Link(std::list<const Shader*>{begin(sh), end(sh)});
        //}
        
        void Activate( ) const;
        static void Deactivate();

        class AttributeBinding : private std::string {
        public:
            using value_type = gl::Int;
            AttributeBinding& operator= (value_type v);
            operator value_type() const;
        private:
            const Name<Program>::value_type _host;
            mutable value_type _value;

            friend class Program;
            template<typename ...Args>
            AttributeBinding(Program::value_type id, Args ...args) : 
                std::string{ std::forward<Args>(args)... },
                _host{ id }, 
                _value{ -1 }
            {}
        };

        class AttributesProxy {
        public:
            AttributesProxy(Program& context) : _host{ context._name } {}
            template <typename arg>
            AttributeBinding operator[] (arg input) { return AttributeBinding{ _host, input }; }
        private:
            Program::value_type _host;
        } attributes{ *this };

        class UniformBinding : private BindingReference<UniformBuffer::BindingPoint> {
        public:
            UniformBinding& operator= (UniformBuffer::BindingPoint value);
            operator UniformBuffer::BindingPoint() const;
        private:
            friend class Program;
            using BindingReference::BindingReference;
        };

        UniformBinding operator[](const std::string& block_name);
        const UniformBinding operator[](const std::string& block_name) const;

        template <typename V>
        class UniformAccessor : private BindingReference<gl::Int> {
        public:
            UniformAccessor& operator= (const V& value);
            
            operator V () const;
        private:
            friend class Program;
            using BindingReference::BindingReference;
        };
        
        template <typename V>
        UniformAccessor<V> Uniform(const std::string& uniformName)
        {
            Int index = glGetUniformLocation(_name, uniformName.c_str());
//            if (index != -1) {
                return UniformAccessor<V>{index, * this};
//            } else {
//                throw std::runtime_error{ "invalid uniform index: " + uniformName };
//            }
//            return UniformAccessor<V>{glGetUniformLocation(_name, uniformName.c_str()), *this};
        }
        
        template <typename V>
        const UniformAccessor<V> Uniform(const std::string& uniformName) const
        {
            return UniformAccessor<V>{glGetUniformLocation(_name, uniformName.c_str()), *this};
        }
        
    private:
        void Link(std::list<Shader const*> sh);
    };
}

#endif
