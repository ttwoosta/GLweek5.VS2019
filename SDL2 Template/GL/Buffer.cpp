//
//  Buffer.cpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/23/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#include "Buffer.h"

namespace gl {
    template<>
    Name<GeneralBuffer>::Name()
    {
        glGenBuffers(1, &_name);
    }
    
    template<>
    Name<GeneralBuffer>::~Name()    {
        if (_name) { glDeleteBuffers(1, &_name); }
    }

    GeneralBuffer::GeneralBuffer(value_type n):
        Name{ n }
    {}
    
    void GeneralBuffer::Activate(GeneralBuffer::Contents target) const
    {
        if (!_name) { throw std::runtime_error("Attempted to activate invalid OpenGL buffer"); }
        glBindBuffer(target, _name);
    }
    
    void GeneralBuffer::Deactivate(GeneralBuffer::Contents target)
    {
        glBindBuffer(target, 0);
    }

	void GeneralBuffer::Release()
    {
		_name = 0;
	}
}
