#ifndef ARITHMETIX_TITLE_HPP
#define ARITHMETIX_TITLE_HPP

#include "state.hpp"
#include "event.hpp"
#include "button.hpp"
#include "sdlw_audio.hpp"

class title : public state
{
  public:
    title(state_machine & sm);

  private:

    void activate_state(const state_arg &);
    void deactivate_state();

    void on_event(const SDL_Event & event);
    void on_draw(sdl_surface & screen);

    void draw_title(sdl_surface & screen);

    sdl_surface     m_bkg;
    sdl_surface     m_title;

    sdlw_sound      m_beep;

    button          m_scores;
    button          m_game;
    button          m_quit;
    button          m_credits;
    state_machine & m_sm;
};

#endif
