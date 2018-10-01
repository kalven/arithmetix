#ifndef ARITHMETIX_TITLE_HPP
#define ARITHMETIX_TITLE_HPP

#include "button.hpp"
#include "event.hpp"
#include "state.hpp"

class title : public state {
 public:
  title(state_machine &sm);

 private:
  void activate_state(const state_arg &);
  void deactivate_state();

  void on_event(const SDL_Event &event);
  void on_draw(sdl_surface &screen);

  void draw_title(sdl_surface &screen);

  sdl_surface m_bkg;
  sdl_surface m_title;

  button m_scores;
  button m_game;
  button m_quit;
  button m_credits;
  state_machine &m_sm;
};

#endif
