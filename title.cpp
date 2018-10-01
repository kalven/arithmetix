#include "title.hpp"
#include "arithmetix.hpp"

#include <cmath>
#include <ctime>

title::title(state_machine &sm)
    : m_bkg(load_image_resource("game.png")),
      m_title(load_image_resource("title.png", with_alpha)),
      m_sm(sm) {
  m_game.pos() = rect(100, 300, 72, 39);
  m_scores.pos() = rect(188, 300, 72, 39);
  m_quit.pos() = rect(276, 300, 72, 39);
  m_credits.pos() = rect(188, 350, 72, 39);

  m_game.set_resource(load_button_resource("play"));
  m_scores.set_resource(load_button_resource("hiscores"));
  m_quit.set_resource(load_button_resource("quit"));
  m_credits.set_resource(load_button_resource("credits"));
}

void title::activate_state(const state_arg &) {
  on_event(get_mouse_motion_event());
}

void title::deactivate_state() {}

void title::on_event(const SDL_Event &event) {
  if (event.type == SDL_MOUSEMOTION) {
    m_scores.motion(event.motion);
    m_game.motion(event.motion);
    m_quit.motion(event.motion);
    m_credits.motion(event.motion);
  }

  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (m_scores.click(event.button)) {
      m_sm.set_next_state("hiscore");
    }
    if (m_game.click(event.button)) {
      game_start_transition gs = {
          static_cast<unsigned int>(std::time(0))};  // Starting seed
      m_sm.set_next_state("game", gs);
    }
    if (m_quit.click(event.button)) {
      SDL_Event quit = get_quit_event();
      SDL_PushEvent(&quit);
    }
    if (m_credits.click(event.button)) {
      m_sm.set_next_state("credits");
    }
  }
}

void title::on_draw(sdl_surface &screen) {
  SDL_BlitSurface(m_bkg, 0, screen, 0);

  draw_title(screen);

  m_scores.draw(screen);
  m_game.draw(screen);
  m_quit.draw(screen);
  m_credits.draw(screen);
}

void title::draw_title(sdl_surface &screen) {
  static const SDL_Rect src[] = {
      {2, 4, 46, 42},    // A
      {53, 4, 40, 42},   // R
      {98, 4, 23, 42},   // I
      {126, 1, 43, 45},  // T
      {175, 4, 49, 42},  // H
      {226, 4, 57, 42},  // M
      {288, 0, 31, 48},  // E
      {126, 1, 43, 45},  // T
      {98, 4, 23, 42},   // I
      {322, 4, 42, 44}   // X
  };

  static const int xadd[] = {
      41,  // A
      40,  // R
      24,  // I
      42,  // T
      46,  // H
      53,  // M
      32,  // E
      42,  // T
      24,  // I
      0    // Not used
  };

  rect dst(25, 0);

  for (int i = 0; i != 10; ++i) {
    dst.y = 100 + src[i].y;
    dst.y +=
        static_cast<int>(std::sin((SDL_GetTicks() + (dst.x)) / 300.0) * 10.0);
    SDL_BlitSurface(m_title, const_cast<SDL_Rect *>(&src[i]), screen, &dst);
    dst.x += xadd[i];
  }
}
