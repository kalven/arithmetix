#ifndef ARITHMETIX_GAME_HPP
#define ARITHMETIX_GAME_HPP

#include "state.hpp"
#include "board.hpp"
#include "queue.hpp"
#include "event.hpp"
#include "status.hpp"
#include "button.hpp"
#include "transition.hpp"
#include "arithmetix.hpp"
#include "sdlw_audio.hpp"

#include <list>

const int board_xpos = 40;
const int board_ypos = 106;
const int queue_xpos = 360;
const int queue_ypos = 106;
const int status_xpos = 80;
const int status_ypos = 30;
const int queue_size = 50;

class tile_anim
{
  public:
    tile_anim(sdl_surface & digits, int digit, int x, int y) :
      m_digits(digits),
      m_frame(0),
      m_digit(digit),
      m_x(x),
      m_y(y)
    {
    }

    bool tick()
    {
        return ++m_frame == 16;
    }

    void draw(sdl_surface & screen)
    {
        SDL_Rect src = { m_digit*32, 0, 32, 32 - m_frame * 2};
        SDL_Rect dest = { board_xpos + m_x * 32, board_ypos + m_y * 32 + m_frame * 2};
        SDL_BlitSurface(m_digits, &src, screen, &dest);
    }

  private:

    sdl_surface & m_digits;
    int m_frame, m_digit, m_x, m_y;
};

class bonus_anim
{
  public:
    bonus_anim(sdl_surface & srf, int cleared, int x, int y) :
      m_srf(srf),
      m_frame(0),
      m_cleared(cleared),
      m_x(x),
      m_y(y)
    {
        assert(m_cleared >= 4 && m_cleared <= 8);
    }

    bool tick()
    {
        return ++m_frame == 64;
    }

    void draw(sdl_surface & screen)
    {
        if(m_frame > 24 || m_frame & 1)
        {
            SDL_Rect src = { 0, (m_cleared-4) * 22, 95, 20 };
            SDL_Rect dest = { m_x, m_y-(m_frame/2) };
            SDL_BlitSurface(m_srf, &src, screen, &dest);
        }
    }

  private:

    sdl_surface & m_srf;
    int m_frame, m_cleared, m_x, m_y;
};

class game : public state
{
  public:
    game(state_machine & sm);

  private:

    struct special_move
    {
        enum type { nuke, shuffle, clear_digit, none };

        special_move()
          : m_type(none)
        {}

        type   m_type;
        button m_button;
    };

    enum { special_nuke, special_shuffle, special_clear_digit, max_special };

    void activate_state(const state_arg & args);

    void on_event(const SDL_Event & event);
    void on_draw(sdl_surface & screen);

    void do_special(special_move& special);
    void do_digit();
    int clear_3x3(int x, int y);

    bool in_board() const;

    void end_game();

    sdl_surface           m_bkg;
    sdl_surface           m_selection;
    sdl_surface           m_digits;
    sdl_surface           m_board_bkg;
    sdl_surface           m_queue_bkg;
    sdl_surface           m_bonus_gfx;
    sdlw_sound            m_clear_snd;
    board                 m_board;
    int                   m_selx,m_sely;
    int                   m_start_time;
    int                   m_bonus;
    queue                 m_queue;
    button                m_quit;
    special_move          m_specials[3];
    special_move *        m_active_special;
    state_machine &       m_sm;
    std::list<tile_anim>  m_anims;
    std::list<bonus_anim> m_bonusanims;
    boost::mt19937        m_twister;
};

#endif
