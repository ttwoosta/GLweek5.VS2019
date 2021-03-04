//
//  SDL.cpp
//
//  Created by Nevin Flanagan on 12/28/15.
//  Copyright © 2015 PlaySmith. All rights reserved.
//

#include "SDL.h"

#include <string>

using std::runtime_error;

sdl::gl::Context sdl::Window::CreateContext() &
{
    auto context = SDL_GL_CreateContext(base());
    if (context) {
        return context;
    }
    throw sdl::Exception{"Failed to create context"};
}

sdl::Library::Library(sdl::Library::Init flags)
{
    if (SDL_Init(flags) < 0) throw sdl::Exception{"Video initialization failed: "};
}

sdl::Ticks sdl::Library::GetTicks() noexcept
{
    return Ticks {SDL_GetTicks()};
}

sdl::Window* sdl::Library::NewWindow(const std::string& title, int left, int top, int width, int height, Window::Features options)
{
    auto window = reinterpret_cast<sdl::Window*>( SDL_CreateWindow(title.c_str(), left, top, width, height, static_cast<Uint32>(options)) );
    if (window) {
        return window;
    }
    throw sdl::Exception{ "Failed to create window" };
}

sdl::Library::~Library()
{
    SDL_Quit();
}

sdl::Exception::Exception(std::string const& message):
    runtime_error(message + SDL_GetError())
{}
