//
//  Display.cpp
//  gfxLite
//
//  Created by Nevin Flanagan on 12/22/20.
//

#include <GL/glew.h>

#include "Display.h"

#include "glm/gtc/type_ptr.hpp"

Display::Display(sdl::Library& context, int width, int height)
{
    context.gl[sdl::gl::ProfileMask] = SDL_GL_CONTEXT_PROFILE_CORE;
    context.gl[sdl::gl::MajorVersion] = 4;
    context.gl[sdl::gl::MinorVersion] = 1;
    
    window = context.NewWindow("", 120, 120, width, height, sdl::Window::OpenGL);
    window->CreateContext();
    
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();
#endif
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, glm::value_ptr(glm::vec4(1.0f)));
    
    glClearColor( 0.0f, 0.0f, 0.4f, 0.0f );
    glClearDepth( 1.0f );
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    
    /* Setup our viewport. */
    glViewport( 0, 0, static_cast<GLsizei>(width), static_cast<GLsizei>(height) );
}

void Display::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::Refresh() const
{
    window->GLSwapWindow();
}
