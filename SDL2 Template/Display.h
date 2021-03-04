//
//  Display.hpp
//  gfxLite
//
//  Created by Nevin Flanagan on 12/22/20.
//

#ifndef Display_hpp
#define Display_hpp

#include "SDL2/SDL.h"

class Display {
public:
    Display(sdl::Library& context, int w, int h);

    void Clear() const;
    void Refresh() const;
private:
    sdl::Window* window;
};

#endif /* Display_hpp */
