#include "button.hpp"

button::button() :
  m_hot(false)
{
}

void button::set_resource(const button_resource & res)
{
    m_res = res;
}

bool button::sample(int x, int y)
{
    if(m_res.bg.surface.null())
        return true;
    SDL_PixelFormat * pf = m_res.bg.surface.format();
    // If there is no alpha channel, the whole image is clickable
    if(pf->BitsPerPixel != 32)
        return true;
    if(x >= m_res.bg.src.w || y >= m_res.bg.src.h)
        return false;

    SDL_Surface * s = static_cast<SDL_Surface*>(m_res.bg.surface);

    sdl_surface_lock lock(m_res.bg.surface);
    Uint32 * pixel = reinterpret_cast<Uint32*>(lock.data() + (y * s->pitch) + (4 * x));
    return ((*pixel) & pf->Amask) >> pf->Ashift > 192;
}

void button::draw(sdl_surface & screen)
{
    surface_region & reg = m_hot ? m_res.bg_hot : m_res.bg;
    SDL_BlitSurface(reg.surface, &reg.src, screen, &m_pos);
    rect fg_dst = offset_rect(m_pos, point(7,8));
    SDL_BlitSurface(m_res.fg.surface, &m_res.fg.src, screen, &fg_dst);
}

rect & button::pos()
{
    return m_pos;
}

void button::motion(const SDL_MouseMotionEvent & me)
{
    if(point_in_rect(m_pos, me.x, me.y))
        m_hot = sample(me.x - m_pos.x, me.y - m_pos.y);
    else
        m_hot = false;
}

bool button::click(const SDL_MouseButtonEvent & me)
{
    if(point_in_rect(m_pos, me.x, me.y))
        return sample(me.x - m_pos.x, me.y - m_pos.y);
    else
        return false;
}
