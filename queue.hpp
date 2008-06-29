#ifndef ARITHMETIX_QUEUE_HPP
#define ARITHMETIX_QUEUE_HPP

#include "sdlw.hpp"
#include "arithmetix.hpp"

#include <boost/random/uniform_smallint.hpp>
#include <boost/random/mersenne_twister.hpp>

#include <vector>
#include <cstdlib>

class queue
{
  public:
    queue(int size) :
      m_size(size),
      m_digits(load_image_resource("digits.png"))
    {
    }

    void setup(boost::mt19937 & src)
    {
        boost::uniform_smallint<int> dist(0, 9);
        m_queue.resize(m_size);

        for(unsigned i = 0; i != m_queue.size(); ++i)
            m_queue[i] = dist(src);
    }

    void draw(sdl_surface & screen, int x, int y)
    {
        std::vector<int>::reverse_iterator iter = m_queue.rbegin();

        SDL_Rect dest = {x, y, 0, 0};
        SDL_Rect src = {0, 0, 32, 32};

        for(int i = 0; i != 5; ++i)
        {
            if(iter != m_queue.rend())
            {
                src.x = *iter * 32;
                SDL_BlitSurface(m_digits, &src, screen, &dest);
                ++iter;
            }

            dest.y += 32;
        }
    }

    int top()
    {
        if(m_queue.empty())
            return -1;
        return m_queue.back();
    }

    int take()
    {
        if(m_queue.empty())
            return -1;
        int value = m_queue.back();
        m_queue.pop_back();
        return value;
    }

    int digits_left()
    {
        return m_queue.size();
    }

  private:

    int m_size;
    std::vector<int> m_queue;
    sdl_surface m_digits;
};

#endif
