#ifndef ARITHMETIX_BUTTON_HPP
#define ARITHMETIX_BUTTON_HPP

#include "sdlw.hpp"

struct button_resource
{
    surface_region bg;
    surface_region bg_hot;
    surface_region fg;
};

class button
{
  public:
    button();

    void set_resource(const button_resource & res);

    bool sample(int x, int y);
    void draw(sdl_surface & screen);

    rect & pos();

    void motion(const SDL_MouseMotionEvent & me);
    bool click(const SDL_MouseButtonEvent & me);

  private:

    button_resource m_res;
    bool            m_hot;
    rect            m_pos;
};

#endif
