#include "credits.hpp"
#include "arithmetix.hpp"
#include "event.hpp"

const wchar_t * csl[] =
{
    L"Arithmetix 1.0",
    L"Code, Graphics: Daniel Andersson",
    L"png, vorbis & sbox loader by Sean Barrett"
};

#define LINECOUNT (sizeof(csl)/sizeof(csl[0]))

credits::credits(state_machine & sm) :
  m_bkg(load_image_resource("game.png")),
  //m_cclogo(load_image_resource("cc.png", with_alpha)),
  m_font("test1"),
  m_text_pos(new int[LINECOUNT]),
  m_sm(sm)
{
    m_continue.set_resource(load_button_resource("continue"));
    m_continue.pos() = rect(340, 380, 72, 39);
}

credits::~credits()
{
    delete [] m_text_pos;
}

void credits::activate_state(const state_arg &)
{
    for(int i = 0; i != LINECOUNT; ++i)
        m_text_pos[i] = 500 + i*140;

    on_event(get_mouse_motion_event());
}

void credits::deactivate_state()
{
}

void credits::on_event(const SDL_Event & event)
{
    if(event.type == SDL_MOUSEMOTION)
    {
        m_continue.motion(event.motion);
    }

    if(event.type == SDL_MOUSEBUTTONDOWN)
    {
        if(m_continue.click(event.button))
            m_sm.set_next_state("title");
    }
}

void credits::on_draw(sdl_surface & screen)
{
    SDL_BlitSurface(m_bkg, 0, screen, 0);
    rect dst(20,330);
    SDL_BlitSurface(m_cclogo, 0, screen, &dst);

    for(int i = 0; i != LINECOUNT; ++i)
    {
        if(m_text_pos[i] > 20)
            m_text_pos[i] -= (m_text_pos[i]) / 10;
        if(m_text_pos[i] < 20)
            m_text_pos[i] = 20;

        m_font.render(screen, m_text_pos[i], 250+i*20, csl[i]);
    }

    m_continue.draw(screen);
}
