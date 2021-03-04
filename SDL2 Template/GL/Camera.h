//
//  Texture.hpp
//  OpenG++
//
//  Created by Nevin Flanagan on 1/16/16.
//  Copyright © 2016 PlaySmith. All rights reserved.
//

#pragma once

#ifndef OPENGL_WRAPPER_CAMERA
#define OPENGL_WRAPPER_CAMERA

#include "OpenGL.h"
#include "Buffer.h"
#include "Shader.h"
#include "Vertex.h"

#include <iostream>
using std::cerr;

namespace gl {
    class Camera : private UniformBuffer {
    public:
        struct View {
            Matrix4 facing, projection;
        };
        Camera(const Matrix4& projection, Matrix4 view = Matrix4{});
        auto Access() { return UniformBuffer::Access<Camera::View>(); }
        Camera& operator << (const Program& pr)&;
        Camera& operator << (const Vertex::Array& vao)&;
        Camera& operator << (Vector3 displacement);
        template <typename T>
        Camera& operator << (std::pair<T, Size> block)&
        {
            glDrawElements(GL_TRIANGLES, block.second, static_cast<GLenum>(TypeSignal<T>), reinterpret_cast<void*>(block.first * sizeof(T)));
            return *this;
        }
    private:
    };

}

#endif
