#include "hiscore.hpp"
#include "event.hpp"
#include "transition.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>

static const char hiscore_file[] = "hiscores.dat";

hiscore::hiscore(state_machine& sm)
    : m_bkg(load_image_resource("game.png")),
      m_edit(15),
      m_sm(sm),
      m_font("test1"),
      m_state(show_hiscores),
      m_current_score_pos(-1) {
  m_continue.set_resource(load_button_resource("continue"));

  if (load_hiscores() == false) {
    // Generate some defaults
    for (int i = 0; i != 10; ++i) {
      hiscore_entry e = {L"Name", 500 * (10 - i)};
      m_scores.push_back(e);
    }
  }

  m_continue.pos() = rect(340, 380, 72, 39);
}

bool hiscore::load_hiscores() {
  std::ifstream scores(hiscore_file, std::ios::binary);
  if (scores.is_open() == false) {
    return false;
  }

  m_scores.clear();
  m_scores.resize(10);

  for (int i = 0; i != 10; ++i) {
    wchar_t name[16] = {};
    int score;
    scores.read(reinterpret_cast<char*>(name), sizeof(name));
    scores.read(reinterpret_cast<char*>(&score), sizeof(score));
    hiscore_entry e = {std::wstring(name), score};
    m_scores[i] = e;
  }

  return true;
}

void hiscore::save_hiscores() {
  std::ofstream scores(hiscore_file, std::ios::binary);
  if (scores.is_open()) {
    for (int i = 0; i != 10; ++i) {
      wchar_t name[16] = {};
      std::copy(m_scores[i].name.begin(), m_scores[i].name.end(), name);
      scores.write(reinterpret_cast<char*>(name), sizeof(name));
      scores.write(reinterpret_cast<char*>(&m_scores[i].score),
                   sizeof(m_scores[i].score));
    }
  }
}

void hiscore::activate_state(const state_arg& args) {
  // Default state to go to
  m_state = show_hiscores;
  m_current_score_pos = -1;

  if (const game_end_transition* t = std::get_if<game_end_transition>(&args)) {
    int score = 0;
    // Move bonus is 100 * no tiles > 3
    score += t->finish * 2000;
    score += t->specials * 200;
    score += t->queue * 25;
    score += t->bonus;
    if (t->finish && (120000 - t->time > 0)) {
      score += (120000 - t->time) / 20;
      std::cout << "Time bonus: " << ((120000 - t->time) / 20) << std::endl;
    }

    std::cout << "Game over, score: " << score << std::endl;
    std::cout << "Play time: " << (t->time / 1000.0) << " seconds."
              << std::endl;

    for (unsigned i = 0; i != m_scores.size(); ++i) {
      if (score >= m_scores[i].score) {
        m_current_score_pos = i;
        hiscore_entry e = {L"", score};
        m_scores.insert(m_scores.begin() + i, e);
        m_scores.resize(m_scores.size() - 1);
        m_edit.show_caret(true);
        m_edit.set_text(L"");
        m_state = edit_name;
        break;
      }
    }
  }

  on_event(get_mouse_motion_event());
}

void hiscore::on_event(const SDL_Event& event) {
  if (event.type == SDL_KEYDOWN && m_state == edit_name) {
    if (event.key.keysym.unicode == 13)  // Enter
    {
      if (m_current_score_pos != -1 &&
          unsigned(m_current_score_pos) < m_scores.size()) {
        m_scores[m_current_score_pos].name = m_edit.get_text();
        if (m_scores[m_current_score_pos].name.empty()) {
          return;  // Not ready to leave the state yet
        }
      }

      m_edit.show_caret(false);
      m_state = show_hiscores;

      save_hiscores();
    } else {
      m_edit.on_event(event.key);
    }
  }

  if (event.type == SDL_MOUSEMOTION) {
    m_continue.motion(event.motion);
  }

  if (event.type == SDL_MOUSEBUTTONDOWN && m_state == show_hiscores) {
    if (m_continue.click(event.button)) m_sm.set_next_state("title");
  }
}

void hiscore::on_draw(sdl_surface& screen) {
  SDL_BlitSurface(m_bkg, 0, screen, 0);

  if (m_state == edit_name) {
    m_edit.draw(screen, 110, 110 + m_current_score_pos * 24);
  } else {
    m_continue.draw(screen);
  }

  for (unsigned i = 0; i != m_scores.size(); ++i) {
    std::wostringstream os;
    os << (i + 1)
       << ((i == unsigned(m_current_score_pos))
               ? L": "
               : L". ");  // TODO: Show the current line nicer
    m_font.render(screen, 80, 110 + i * 24, os.str());
    os.str(L"");
    m_font.render(screen, 110, 110 + i * 24, m_scores[i].name);
    os << m_scores[i].score;
    int score_width = m_font.get_width(os.str());
    m_font.render(screen, 300 - score_width, 110 + i * 24, os.str());
  }
}
