#include "game.hpp"

#include <cmath>
#include <algorithm>

game::game(state_machine & sm) :
  m_bkg(load_image_resource("game.png")),
  m_selection(load_image_resource("selection.png", with_alpha)),
  m_digits(load_image_resource("digits.png")),
  m_board_bkg(load_image_resource("board.png", with_alpha)),
  m_queue_bkg(load_image_resource("queue.png", with_alpha)),
  m_bonus_gfx(load_image_resource("bonus.png", with_alpha)),
  m_clear_snd(load_sound_resource("sounds/button.ogg")),
  m_selx(-1),
  m_sely(-1),
  m_queue(queue_size),
  m_active_special(0),
  m_sm(sm)
{
    m_quit.pos() = rect(340,380,72,39);
    m_quit.set_resource(load_button_resource("quit"));
    m_specials[0].m_button.pos() = rect(60,40,72,39);
    m_specials[0].m_button.set_resource(load_button_resource("quit"));
    m_specials[1].m_button.pos() = rect(150,40,72,39);
    m_specials[1].m_button.set_resource(load_button_resource("quit"));
    m_specials[2].m_button.pos() = rect(240,40,72,39);
    m_specials[2].m_button.set_resource(load_button_resource("quit"));
}

void game::activate_state(const state_arg & args)
{
    m_anims.clear();
    m_bonusanims.clear();

    if(const game_start_transition * g = boost::get<game_start_transition>(&args))
    {
        m_twister.seed(boost::mt19937::result_type(g->seed));
        m_board.setup(m_twister);
        m_queue.setup(m_twister);
        m_start_time = SDL_GetTicks();
        m_bonus = 0;
        m_specials[0].m_type = special_move::shuffle;
        m_specials[1].m_type = special_move::nuke;
        m_specials[2].m_type = special_move::clear_digit;
        m_active_special = 0;
        on_event(get_mouse_motion_event());
    }
    else
        assert(0 && "game::activate_state called with invalid args");
}

void game::on_event(const SDL_Event & event)
{
    if(event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if(in_board())
        {
            if(m_active_special)
            {
                const int digit = m_board.get_digit(m_selx, m_sely);

                // Attempt a special move
                if(m_active_special->m_type == special_move::nuke
                    && digit == -1)
                {
                    clear_3x3(m_selx, m_sely);
                    m_active_special->m_type = special_move::none;
                }

                if(m_active_special->m_type == special_move::clear_digit
                    && digit != -1)
                {
                    for(int y = 0; y != 9; ++y)
                        for(int x = 0; x != 9; ++x)
                            if(m_board.get_digit(x, y) == digit)
                                m_board.set_digit(x, y, -1);

                    m_active_special->m_type = special_move::none;
                }

                m_active_special = 0;
            }
            else
            {
                do_digit();
            }

            if(m_queue.digits_left() == 0 || m_board.get_filled() == 0)
                end_game();
        }

        for(int i = 0; i != 3; ++i)
            if(m_specials[i].m_button.click(event.button) && m_specials[i].m_type != special_move::none)
                do_special(m_specials[i]);

        if(m_quit.click(event.button))
            m_sm.set_next_state("title");
    }

    if(event.type == SDL_MOUSEMOTION)
    {
        // Check if the cursor is inside the board
        int mx = event.motion.x;
        int my = event.motion.y;

        if(mx >= board_xpos && mx < (board_xpos+32*9) &&
           my >= board_ypos && my < (board_ypos+32*9))
        {
            m_selx = (mx - board_xpos) / 32;
            m_sely = (my - board_ypos) / 32;
        }
        else
        {
            m_selx = -1;
            m_sely = -1;
        }

        m_quit.motion(event.motion);

        for(int i = 0; i != 3; ++i)
            if(m_specials[i].m_type != special_move::none)
                m_specials[i].m_button.motion(event.motion);
    }
}

void game::on_draw(sdl_surface & screen)
{
    SDL_BlitSurface(m_bkg, 0, screen, 0);

    // TODO: Do this once to a surface at game instanciation
    SDL_Rect board_bkg_dest = {34,100};
    SDL_BlitSurface(m_board_bkg, 0, screen, &board_bkg_dest);
    SDL_Rect queue_bkg_dest = {354,100};
    SDL_BlitSurface(m_queue_bkg, 0, screen, &queue_bkg_dest);

    m_board.draw(screen, board_xpos, board_ypos);
    m_queue.draw(screen, queue_xpos, queue_ypos);

    for(std::list<tile_anim>::iterator ai = m_anims.begin(); ai != m_anims.end();)
    {
        ai->draw(screen);
        if(ai->tick())
            ai = m_anims.erase(ai);
        else
            ++ai;
    }

    if(in_board() && m_board.get_digit(m_selx, m_sely) == -1)
    {
        // Draw the selection
        int lx = std::max(0, m_selx - 1); int hx = std::min(9, m_selx + 2);
        int ly = std::max(0, m_sely - 1); int hy = std::min(9, m_sely + 2);

        // Draw corners
        SDL_Rect dest, src = {0, 0, 8, 8}; // Top left
        dest.x = board_xpos + lx * 32 - 8; dest.y = board_ypos + ly * 32 - 8;
        SDL_BlitSurface(m_selection, &src, screen, &dest);
        src.x = 104; // Top right
        dest.x = board_xpos + hx * 32; dest.y = board_ypos + ly * 32 - 8;
        SDL_BlitSurface(m_selection, &src, screen, &dest);
        src.y = 104; // Bottom right
        dest.x = board_xpos + hx * 32; dest.y = board_ypos + hy * 32;
        SDL_BlitSurface(m_selection, &src, screen, &dest);
        src.x = 0; // Bottom left
        dest.x = board_xpos + lx * 32 - 8; dest.y = board_ypos + hy * 32;
        SDL_BlitSurface(m_selection, &src, screen, &dest);

        // Draw top & bottom
        src.x = 8; src.w = 32;
        for(int x = lx; x != hx; ++x) // Top & bottom
        {
            src.y = 0;
            dest.x = board_xpos + x * 32; dest.y = board_ypos + ly * 32 - 8;
            SDL_BlitSurface(m_selection, &src, screen, &dest);
            src.y = 104;
            dest.y = board_ypos + hy * 32;
            SDL_BlitSurface(m_selection, &src, screen, &dest);
        }
        src.y = 8; src.w = 8; src.h = 32;
        for(int y = ly; y != hy; ++y) // Left & right
        {
            src.x = 0;
            dest.x = board_xpos + lx * 32 - 8; dest.y = board_ypos + y * 32;
            SDL_BlitSurface(m_selection, &src, screen, &dest);
            src.x = 104;
            dest.x = board_xpos + hx * 32;
            SDL_BlitSurface(m_selection, &src, screen, &dest);
        }

        int queue_digit = m_queue.top();
        if(queue_digit != -1)
        {
            // Fade the queue digit in a pleasant manner
            int digit_alpha = 96 + static_cast<int>(std::sin(SDL_GetTicks() / 300.0) * 48);
            SDL_Rect src = { queue_digit * 32, 0, 32, 32};
            SDL_Rect dest = { board_xpos + m_selx * 32, board_ypos + m_sely * 32};
            SDL_SetAlpha(m_digits, SDL_SRCALPHA, digit_alpha);
            SDL_BlitSurface(m_digits, &src, screen, &dest);
            SDL_SetAlpha(m_digits, 0, 255);
        }
    }

    m_quit.draw(screen);

    for(int i = 0; i != 3; ++i)
        if(m_specials[i].m_type != special_move::none)
            m_specials[i].m_button.draw(screen);

    for(std::list<bonus_anim>::iterator ai = m_bonusanims.begin(); ai != m_bonusanims.end();)
    {
        ai->draw(screen);
        if(ai->tick())
            ai = m_bonusanims.erase(ai);
        else
            ++ai;
    }
}

void game::do_special(special_move& special)
{
    switch(special.m_type)
    {
        case special_move::none:
            break;
        case special_move::nuke:
        case special_move::clear_digit:
            // These require further input from the user. In the case
            // of a nuke, an empty square must be selected. In the
            // case of clear_digit, a digit must be selected. Here we
            // just mark the special move as "active".
            m_active_special = &special;
            break;
        case special_move::shuffle:
        {
            // Collect ...
            std::vector<int> digits;
            for(int y = 0; y != 9; ++y)
                for(int x = 0; x != 9; ++x)
                {
                    int digit = m_board.get_digit(x, y);
                    if(digit != -1)
                        digits.push_back(m_board.get_digit(x, y));
                }

            // randomize
            std::random_shuffle(digits.begin(), digits.end());

            // ... and distribute
            std::vector<int>::const_iterator i = digits.begin();
            for(int y = 0; y != 9; ++y)
                for(int x = 0; x != 9; ++x)
                    if(m_board.get_digit(x, y) != -1)
                        m_board.set_digit(x, y, *i++);

            special.m_type = special_move::none;
            break;
        }
    }
}

void game::do_digit()
{
    if(in_board() && m_board.get_digit(m_selx, m_sely) == -1)
    {
        int board_digit = m_board.get_3x3_sum(m_selx, m_sely) % 10;
        int queue_digit = m_queue.take();

        if(board_digit == queue_digit)
        {
            // Ding, clear the 3x3 square
            int cleared = clear_3x3(m_selx, m_sely);
            if(cleared > 3)
            {
                m_bonus += cleared * 100;
                m_bonusanims.push_back(bonus_anim(m_bonus_gfx, cleared, board_xpos + (m_selx*32) - 29, board_ypos + (m_sely*32) - 8));
            }

            play_sample(m_clear_snd);
        }
        else
        {
            // Oh noes, place the queue digit
            m_board.set_digit(m_selx, m_sely, queue_digit);
            if(m_board.get_cleared() == 0)
                end_game();
        }
    }
}

int game::clear_3x3(int xpos, int ypos)
{
    int cleared = 0;
    int lx = std::max(0,xpos-1); int hx = std::min(9,xpos+2);
    int ly = std::max(0,ypos-1); int hy = std::min(9,ypos+2);

    for(int y = ly; y != hy; ++y)
        for(int x = lx; x != hx; ++x)
        {
            const int digit = m_board.get_digit(x,y);
            if(digit != -1)
            {
                cleared++;
                m_anims.push_back(tile_anim(m_digits, digit, x, y));
                m_board.set_digit(x, y, -1);
            }
        }

    return cleared;
}

bool game::in_board() const
{
    return m_selx != -1 && m_sely != -1;
}

void game::end_game()
{
    game_end_transition t;

    t.finish   = m_board.get_filled() == 0;
    t.bonus    = m_bonus;
    t.queue    = m_queue.digits_left();
    t.time     = SDL_GetTicks() - m_start_time;
    t.specials = 0;
    for(int i = 0; i != 3; ++i)
        t.specials += int(m_specials[i].m_type != special_move::none);

    // Transition to hiscore state
    m_sm.set_next_state("hiscore", t);
}
