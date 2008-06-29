#ifndef ARITHMETIX_HISCORE_HPP
#define ARITHMETIX_HISCORE_HPP

#include "state.hpp"
#include "arithmetix.hpp"
#include "transition.hpp"
#include "button.hpp"
#include "edit.hpp"
#include "font.hpp"

#include <string>
#include <vector>

class hiscore : public state
{
  public:
    hiscore(state_machine & sm);

  private:

    struct hiscore_entry
    {
        std::wstring name;
        int score;
    };

    enum hiscore_state
    {
        show_hiscores,
        edit_name
    };

    bool load_hiscores();
    void save_hiscores();

    void activate_state(const state_arg & args);
    void on_event(const SDL_Event & event);
    void on_draw(sdl_surface & screen);

    typedef std::vector<hiscore_entry> hiscores;

    sdl_surface     m_bkg;
    editbox         m_edit;
    button          m_continue;
    state_machine & m_sm;
    font            m_font;
    hiscores        m_scores;
    hiscore_state   m_state;
    int             m_current_score_pos;
};

#endif
