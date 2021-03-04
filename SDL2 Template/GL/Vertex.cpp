//
//  Vertex.cpp
//  GLhomework1
//
//  Created by Nevin Flanagan on 1/31/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Vertex.h"

namespace gl {
    template<>
    Name<Vertex::Array>::Name() 
	{
        glGenVertexArrays(1, &_name);
    }
    
    template<>
    Name<Vertex::Array>::~Name() 
	{
		if (_name) { glDeleteVertexArrays(1, &_name); }
    }

    Vertex::Array& Vertex::Array::operator << (ElementArrayBuffer& edges)&
    {
        Activate();
        edges.Activate();
        return *this;
    }

    void Activate(Vertex::Array& target)
    {
        target.Activate();
    }

    void Deactivate(Vertex::Array&)
    {
        Vertex::Array::Deactivate();
    }

    Vertex::BufferAttribute::~BufferAttribute()
    {
        glDisableVertexAttribArray(_channel);
    }

    Vertex::Array& Vertex::BufferAttribute::operator>>=(Array& source) const
    {
        source.Activate();
        _contents.Activate();
        glEnableVertexAttribArray(_channel);
        glVertexAttribPointer(_channel, _count, static_cast<GLenum>(_type), _normalized, _stride, reinterpret_cast<void*>(_offset));
        return source;
    }

    void Vertex::Array::Activate() const
	{
        glBindVertexArray(_name);
    }
    
    void Vertex::Array::Deactivate() 
	{
        glBindVertexArray(0);
    }
}
