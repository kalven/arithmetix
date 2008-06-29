#ifndef SDLW_HPP
#define SDLW_HPP

#include <SDL/SDL.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <string>

class sdl_surface
{
  public:
    sdl_surface() :
      m_surface(0)
    {
    }

    sdl_surface(SDL_Surface * s) :
      m_surface(s)
    {
        if(s == 0)
            throw std::logic_error("sdlw error: surface was null");
    }

    sdl_surface(const sdl_surface & rhs) :
      m_surface(rhs.m_surface)
    {
        m_surface->refcount++;
    }

    sdl_surface & operator=(const sdl_surface & rhs)
    {
        if(this != &rhs)
        {
            sdl_surface tmp(rhs);
            swap(tmp);
        }

        return *this;
    }

    ~sdl_surface()
    {
        if(m_surface)
            SDL_FreeSurface(m_surface);
    }

    void swap(sdl_surface & rhs)
    {
        std::swap(m_surface, rhs.m_surface);
    }

    SDL_PixelFormat * format() const
    {
        assert(m_surface != 0 && "sdl_surface::format called on null surface");
        return m_surface->format;
    }

    int width() const
    {
        return m_surface ? m_surface->w : 0;
    }

    int height() const
    {
        return m_surface ? m_surface->h : 0;
    }

    bool null() const
    {
        return m_surface == 0;
    }

    operator SDL_Surface *() const
    {
        return m_surface;
    }

    friend std::ostream & operator<<(std::ostream &, const sdl_surface &);

  private:

    SDL_Surface * m_surface;
};

class sdl_surface_lock
{
  public:
    sdl_surface_lock(sdl_surface & s) :
      m_surf(s)
    {
        if(SDL_MUSTLOCK(static_cast<SDL_Surface*>(m_surf)))
            SDL_LockSurface(m_surf);
    }

    ~sdl_surface_lock()
    {
        if(SDL_MUSTLOCK(static_cast<SDL_Surface*>(m_surf)))
            SDL_UnlockSurface(m_surf);
    }

    Uint8 * data()
    {
        return reinterpret_cast<Uint8*>(static_cast<SDL_Surface*>(m_surf)->pixels);
    }

  private:

    sdl_surface & m_surf;
};

struct point
{
    point(int x_, int y_) :
      x(x_), y(y_)
    {
    }

    point() :
      x(0), y(0)
    {
    }

    int x,y;
};

struct rect : public SDL_Rect
{
    rect(const point & p)
    {
        x = p.x; y = p.y; w = 0; h = 0;
    }

    rect(Sint16 x_, Sint16 y_, Uint16 w_, Uint16 h_)
    {
        x = x_; y = y_; w = w_; h = h_;
    }

    rect(Sint16 x_, Sint16 y_)
    {
        x = x_; y = y_; w = 0; h = 0;
    }

    rect()
    {
        x = 0; y = 0; w = 0; h = 0;
    }
};

inline rect offset_rect(const rect & r, const rect & offset)
{
    return rect(r.x + offset.x, r.y + offset.y, r.w, r.h);
}

inline rect offset_rect(const rect & r, const point & offset)
{
    return rect(r.x + offset.x, r.y + offset.y, r.w, r.h);
}

inline bool point_in_rect(const rect & r, int x, int y)
{
    return x >= r.x && y >= r.y && x <= r.x+r.w && y <= r.y+r.h;
}

inline bool point_in_rect(const rect & r, const point & p)
{
    return point_in_rect(r, p.x, p.y);
}

struct surface_region
{
    sdl_surface surface;
    rect        src;
};

sdl_surface load_image(std::string filename, bool alpha, bool convert);

#endif
