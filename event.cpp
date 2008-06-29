#include "event.hpp"

SDL_Event get_mouse_motion_event()
{
    int x, y;

    SDL_Event e;
    e.type = SDL_MOUSEMOTION;
    e.motion.type = SDL_MOUSEMOTION;
    e.motion.state = SDL_GetMouseState(&x, &y);
    e.motion.x = x;
    e.motion.y = y;

    return e;
}

SDL_Event get_quit_event()
{
    SDL_Event e;

    e.type = SDL_QUIT;
    e.quit.type = SDL_QUIT;

    return e;
}
