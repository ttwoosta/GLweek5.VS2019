//
//  SDL.h
//
//  Created by Nevin Flanagan on 12/28/15.
//  Copyright © 2015 PlaySmith. All rights reserved.
//

#pragma once

#ifndef SDL_CPP_CORE
#define SDL_CPP_CORE

#include <GL/glew.h>

#include <string>
#include <chrono>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_opengl.h>

namespace sdl {
    namespace gl {
        using Context = SDL_GLContext;
        enum attr : std::underlying_type<SDL_GLattr>::type {
            ProfileMask = SDL_GL_CONTEXT_PROFILE_MASK,
            MajorVersion = SDL_GL_CONTEXT_MAJOR_VERSION,
            MinorVersion = SDL_GL_CONTEXT_MINOR_VERSION,
        };
    }

    class Window {
    public:
        enum Features : Uint32 {
            OpenGL = SDL_WINDOW_OPENGL,
        };
        
        gl::Context CreateContext() & ;
        void GLSwapWindow() & { SDL_GL_SwapWindow(base()); }
    private:
        const SDL_Window* base() const & { return reinterpret_cast<const SDL_Window*>(this); }
        SDL_Window* base() & { return reinterpret_cast<SDL_Window*>(this); }
    };

    using Ticks = std::chrono::duration<Uint32, std::chrono::milliseconds::period>;

    class Exception: public std::runtime_error {
    public:
        Exception(std::string const& message);
    };
        
    using Event = SDL_Event;

    enum struct EventType : decltype(sdl::Event::type) {
        Quit = SDL_QUIT,
    };

    class Library {
    public:
        enum Init : Uint32 {
            Video = SDL_INIT_VIDEO,
        };
        Library(Init flags);
        ~Library();
        
        Ticks GetTicks() noexcept;

        template <typename C>
        void ProcessEvents(const C& lookup) const
        {
            Event ev;

            while (SDL_PollEvent(&ev)) {
                auto ty = static_cast<EventType>(ev.type);
                if (lookup.count(ty) > 0) {
                    lookup.at(ty)(ev);
                }
            }
        }

        class {
        public:
            class _binding {
            public:
                _binding(gl::attr title) : title{title} {}
                const gl::attr title;
                operator int() const { int value; SDL_GL_GetAttribute(static_cast<SDL_GLattr>(title), &value); return value; }
                _binding& operator = (int value) { SDL_GL_SetAttribute(static_cast<SDL_GLattr>(title), value); return *this; }
            };
            
            _binding operator[](gl::attr title) & { return _binding{title}; }
            
            void SetAttribute(gl::attr title, int value) { int result = SDL_GL_SetAttribute(static_cast<SDL_GLattr>(title), value); if (result) { throw Exception {"Failed to set attribute"}; } }
            
        } gl;

        Window* NewWindow(const std::string& title, int left, int top, int width, int height, Window::Features options);

    };
}

#endif
