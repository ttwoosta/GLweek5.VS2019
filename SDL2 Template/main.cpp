//
//  main.cpp
//  gfxLite
//
//  Created by Nevin Flanagan on 12/21/20.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <glm/gtx/string_cast.hpp>

using namespace std::string_literals;

std::string file_contents(std::string filename)
{
    using namespace std;
    ifstream in{ filename, ios::in | ios::binary };
    if (in) {
        ostringstream contents;
        contents << in.rdbuf();
        return contents.str();
    }
    throw (errno);
}

#include <SDL2/SDL.h>
#include "glm/gtc/matrix_transform.hpp"

#include "Display.h"
#include "GL/Shader.h"
#include "GL/Vertex.h"
#include "GL/Camera.h"
#include "GL/Mesh.h"

#include "format/OBJ.h"

namespace gfx = gl;

class Project {
public:
    Project(int w, int h);
    int operator()(const std::vector<std::string>& args);
protected:
    using seconds = std::chrono::duration<float>;
    void update(seconds frame, seconds total);
    void render() const;
private:
    sdl::Library SDL;
    Display output;
    bool running;
    mutable gfx::Program renderer;
    mutable gfx::Camera cam;
    std::vector<gfx::Object*> objects;
    
    void KeyboardKeyDown(SDL_KeyboardEvent kEvent);
    SDL_Keycode keySym = SDLK_0;
};

template <typename E, typename C = E>
using ArrayAttribute = gfx::Vertex::ArrayAttribute<E, C>;

Project::Project(int w, int h)
:   SDL {sdl::Library::Video}, 
    output {SDL, w, h}, 
    running {true},
    renderer { 
        gfx::Shader{ gfx::Shader::Vertex, file_contents("plain_vertex.glsl") }, 
        gfx::Shader{ gfx::Shader::Fragment, file_contents("plain_fragment.glsl") } 
    },
    cam {glm::perspectiveLH(1.0f, 4.0f / 3.0f, 1.0f, 200.0f), 
         glm::lookAtLH(gfx::Point3{-7, 2, -3},
         gfx::Point3{0}, 
         gfx::Vector3{0, 1, 0}) 
    }
{ 
    renderer.attributes["position"] = 0;
    renderer.attributes["color"] = 2;
    renderer.attributes["normal"] = 1;
    glVertexAttrib4f(renderer.attributes["color"], 1, 1, 1, 1);
    glVertexAttrib3f(renderer.attributes["normal"], 0, 0, 0);

    auto cube = std::make_shared<gfx::Mesh>();
    *cube <<= geometry::OBJ{ "cube.obj" };

    objects.push_back(new gfx::Object{ cube, renderer });
    objects.back()->transform = glm::translate(gfx::Matrix4{}, { 3, 0, 0 });
    objects.push_back(new gfx::Object{ cube, renderer });
    objects.back()->transform = glm::translate(gfx::Matrix4{}, { 0, 2.5f, 0 });

    auto octo = std::make_shared<gfx::Mesh>();
    *octo <<= geometry::OBJ{ "octo.obj" };

    objects.push_back(new gfx::Object{ octo, renderer });
}

static float m_farClipping = 8.9f;
int Project::operator()(const std::vector<std::string>& args)
{
    using std::chrono::duration_cast;
    using namespace std;
    SDL_Event event;

    cam.Access()->projection = glm::orthoLH(-6.0f, 6.0f, -4.5f, 4.5f, 1.0f, m_farClipping);

    sdl::Ticks previous = SDL.GetTicks();
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT)
                running = false; 

            if (event.type == SDL_MOUSEWHEEL)
            {
                if (event.wheel.y > 0) // scroll up
                {
                    m_farClipping += 0.1f;
                    cam.Access()->projection = glm::orthoLH(-6.0f, 6.0f, -4.5f, 4.5f, 1.0f, m_farClipping);
                }
                else if (event.wheel.y < 0) // scroll down
                {
                    m_farClipping -= 0.1f;
                    cam.Access()->projection = glm::orthoLH(-6.0f, 6.0f, -4.5f, 4.5f, 1.0f, m_farClipping);
                }
            }

            if (event.type == SDL_KEYDOWN) {
                keySym = event.key.keysym.sym;
                KeyboardKeyDown(event.key);
            }
            
            
        }
        sdl::Ticks now = SDL.GetTicks();
        update(duration_cast<seconds>(now - previous), duration_cast<seconds>(now));
        previous = now;
        render();
    }
    return 0;
}

void Project::update(seconds frame, seconds total)
{
    std::unordered_map<sdl::EventType, std::function<void(const sdl::Event&)>> responses;
    responses.emplace(sdl::EventType::Quit, [this](const sdl::Event&) { running = false; });
    SDL.ProcessEvents(responses);
}

void Project::render() const
{
    constexpr gfx::Ushort start = 0;
    output.Clear();
    
    for (auto& object : objects) { cam << *object; }

    output.Refresh();
}


void Project::KeyboardKeyDown(SDL_KeyboardEvent kEvent)
{
    using namespace std;

    if (keySym == SDLK_DOWN)
        cam << gfx::Vector3{ 0,0, 0.2f };
    else if (keySym == SDLK_UP)
        cam << gfx::Vector3{ 0,0,-0.2f };

    if (keySym == SDLK_LEFT)
        cam << gfx::Vector3{ 0,0.2f,0 };
    else if (keySym == SDLK_RIGHT)
        cam << gfx::Vector3{ 0,-0.2f,0 };

    if (keySym == SDLK_1)
        cam << gfx::Vector3{ 0.2f,0,0 };
    else if (keySym == SDLK_3)
        cam << gfx::Vector3{ -0.2f,0,0 };


    if (keySym == SDLK_r) {
        cam.Access()->facing = glm::translate(gl::Matrix4{}, glm::vec3(0, 0, 5));
    }
    else if (keySym == SDLK_t) {
        cam.Access()->facing = glm::translate(gl::Matrix4{}, glm::vec3(-7, 2, -3));
    }

    if (keySym == SDLK_SPACE) {
        auto c = cam.Access();
        cout << "Facing: " << glm::to_string(c->facing) << endl;
        cout << "Far clipping plane: " << m_farClipping << endl;
    }
    else if (keySym == SDLK_p) {
        auto c = cam.Access();
        cout << "Projection: " << glm::to_string(c->projection) << endl;
    }
}

int main(int argc, char* argv[])
{
    using namespace std;
    cout << "Author: Tu Tong (0262620)\n";
    cout << "Use arrow keys to rotate the cube or\n";
    cout << "Press R key to reset position\n";
    cout << "Press Space key to print the camera position\n";
    cout << "Press Mouse scroll up/down to adjust the far clipping plane\n";

    try {
        return Project{1280, 960}(vector<string>{argv, argv + argc});
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
