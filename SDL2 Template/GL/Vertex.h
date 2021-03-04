//
//  Vertex.hpp
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/31/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#ifndef OPENGL_WRAPPER_VERTEX
#define OPENGL_WRAPPER_VERTEX

#include "OpenGL.h"
#include "Buffer.h"

#include <unordered_map>

namespace gl {
    struct Vertex {
        class Array: public Name<Vertex::Array> {
        public:
            Array() = default;
            constexpr explicit Array(nullptr_t) : Name{ nullptr } {}

            Array& operator << (void (*source)(Array&))& { source(*this); return *this; }
            Array& operator << (ElementArrayBuffer& edges)&;
            
            void Activate() const;

            static void Deactivate();
        };
    
        struct BufferAttribute {
            ~BufferAttribute();
            friend Array& operator <<(Array& target, const BufferAttribute& source) {
                return source >>= target;
            }
        protected:
            BufferAttribute(ArrayBuffer& contents, Int channel, Uint count, TypeCode type, Bool normalized, Size stride, Size inset = 0) :
                _contents{ contents },
                _channel{ channel },
                _count{ count },
                _type{ type },
                _normalized{ normalized },
                _stride{ stride },
                _offset{ inset }
            {}
        private:
            ArrayBuffer& _contents;
            Int _channel;
            Uint _count;
            Bool _normalized;
            Size _stride;
            Size _offset;
            TypeCode _type;

            Array& operator>>=(Array&) const;
        };

        template <typename Element, typename Container = Element>
        struct ArrayAttribute : public BufferAttribute {
            ArrayAttribute(Int channel, Size inset, Bool normalized, ArrayBuffer& contents) :
                BufferAttribute { 
                    contents, 
                    channel, 
                    std::is_array<Element>::value ? std::extent<Element, 0>::value : 1,
                    TypeSignal<std::remove_extent<Element>::type>,
                    normalized,
                    sizeof(Container), 
                    inset 
                }
            {}
            ArrayAttribute(Int channel, ArrayBuffer& contents) :
                ArrayAttribute(channel, 0, GL_FALSE, contents)
            {}
            ArrayAttribute(Int channel, Size inset, ArrayBuffer& contents) :
                ArrayAttribute(channel, inset, GL_FALSE, contents)
            {}
        };
        
    };

    void Activate(Vertex::Array& target);
    void Deactivate(Vertex::Array&);
}

#endif
