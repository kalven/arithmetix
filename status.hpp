#ifndef ARITHMETIX_STATUS_HPP
#define ARITHMETIX_STATUS_HPP

#include "arithmetix.hpp"
#include "font.hpp"

#include <sstream>

class status
{
  public:
    status() :
      m_font("scorefont"),
      m_moves(0),
      m_queue_size(0),
      m_board_left(0)
    {
    }

    void set_queue_size(int n)
    {
        m_queue_size = n;
    }

    void set_board_left(int n)
    {
        m_board_left = n;
    }

    void draw(sdl_surface & screen, int xpos, int ypos)
    {
        std::wostringstream os;

        os << L"Queue left: " << m_queue_size;
        m_font.render(screen, xpos, ypos, os.str()); os.str(L"");
        os << L"Board left: " << m_board_left;
        m_font.render(screen, xpos, ypos+20, os.str());
    }

  private:

    font m_font;

    int m_moves;
    int m_queue_size;
    int m_board_left;
};

#endif
