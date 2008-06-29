#ifndef ARITHMETIX_BOARD_HPP
#define ARITHMETIX_BOARD_HPP

#include "sdlw.hpp"
#include "arithmetix.hpp"

#include <boost/random/uniform_smallint.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <algorithm>
#include <cstdlib>
#include <cassert>

class board
{
  public:
    board() :
      m_digits(load_image_resource("digits.png"))
    {
        clear();
    }

    void setup(boost::mt19937 & src)
    {
        boost::uniform_smallint<int> dist(0, 9);

        clear();
        for(int y = 1; y != 8; ++y)
            for(int x = 1; x != 8; ++x)
                gb(x, y) = dist(src);
    }

    void clear()
    {
        std::fill(m_board, m_board+81, -1);
    }

    int get_3x3_sum(int x, int y)
    {
        int sum = 0;
        int lx = std::max(0,x-1); int hx = std::min(9,x+2);
        int ly = std::max(0,y-1); int hy = std::min(9,y+2);

        for(y = ly; y != hy; ++y)
            for(x = lx; x != hx; ++x)
            {
                const int digit = gb(x, y);
                if(digit != -1)
                    sum += digit;
            }

        return sum;
    }

    int get_digit(int x, int y)
    {
        return gb(x, y);
    }

    void set_digit(int x, int y, int digit)
    {
        gb(x,y) = digit;
    }

    int get_cleared()
    {
        return std::count(m_board, m_board+81, -1);
    }

    int get_filled()
    {
        return 81 - get_cleared();
    }

    void draw(sdl_surface & screen, int board_xpos, int board_ypos)
    {
        SDL_Rect dest = {0,0,32,32};
        SDL_Rect src  = {0,0,32,32};

        for(int y = 0; y != 9; ++y)
        {
            for(int x = 0; x != 9; ++x)
            {
                dest.x = board_xpos + x * 32;
                dest.y = board_ypos + y * 32;

                int tile = gb(x, y);
                if(tile != -1)
                {
                    src.x = tile*32;
                    SDL_BlitSurface(m_digits, &src, screen, &dest);
                }
                else
                {
                    //src.x = 0;
                    //SDL_BlitSurface(m_empty, &src, screen, &dest);
                }
            }
        }
    }

  private:

    int & gb(int x, int y)
    {
        assert(x >= 0);
        assert(y >= 0);
        assert(x < 9);
        assert(y < 9);
        return m_board[y * 9 + x];
    }

    int m_board[81];
    sdl_surface m_digits;
};

#endif
