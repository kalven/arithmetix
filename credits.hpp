#ifndef ARITHMETIX_CREDITS_HPP
#define ARITHMETIX_CREDITS_HPP

#include "state.hpp"
#include "font.hpp"
#include "button.hpp"

class credits : public state
{
  public:
    credits(state_machine & sm);
    ~credits();

    void activate_state(const state_arg &);
    void deactivate_state();

    void on_event(const SDL_Event &);
    void on_draw(sdl_surface & screen);

  private:

    sdl_surface     m_bkg;
    sdl_surface     m_cclogo;
    button          m_continue;
    font            m_font;
    int           * m_text_pos;
    state_machine & m_sm;
};

#endif
