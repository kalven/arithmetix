#ifndef ARITHMETIX_EDIT_HPP
#define ARITHMETIX_EDIT_HPP

#include "arithmetix.hpp"
#include "font.hpp"

#include <cmath>
#include <string>

class editbox {
 public:
  editbox(unsigned max_size)
      : m_font("test1"),
        m_caretimg(load_image_resource("caret.png")),
        m_caret(0),
        m_max(max_size),
        m_show_caret(true) {}

  void draw(sdl_surface& screen, int xpos, int ypos) {
    m_font.render(screen, xpos, ypos, m_text);

    if (m_show_caret) {
      int caret_alpha =
          192 + static_cast<int>(std::sin(SDL_GetTicks() / 100.0) * 63);
      int caret_pos = m_font.get_width(m_text, m_caret);
      SDL_Rect caret_dest = {Sint16(xpos + caret_pos), Sint16(ypos + 3)};
      SDL_SetAlpha(m_caretimg, SDL_SRCALPHA, caret_alpha);
      SDL_BlitSurface(m_caretimg, 0, screen, &caret_dest);
      SDL_SetAlpha(m_caretimg, SDL_SRCALPHA, 255);
    }
  }

  void on_event(const SDL_KeyboardEvent& e) {
    wchar_t chr = e.keysym.unicode;

    if (chr != 0) {
      switch (chr) {
        case 8:  // Backspace
          if (m_caret != 0 && !m_text.empty()) {
            m_caret--;
            m_text.erase(m_text.begin() + m_caret);
          }
          break;
        case 127:  // Delete, nix
          if (m_caret != m_text.size()) {
            m_text.erase(m_text.begin() + m_caret);
          }
          break;
        default:  // Printable, maybe
          if (m_font.has_char(chr) && m_text.size() != m_max) {
            m_text.insert(m_text.begin() + m_caret, chr);
            m_caret++;
          }
      }
    } else {
      switch (e.keysym.scancode) {
        case 75:
        case 100:
          if (m_caret != 0) m_caret--;  // Caret left
          break;
        case 77:
        case 102:
          if (m_caret < m_text.size()) {
            m_caret++;
          }
          break;  // Caret right
        case 71:
        case 97:
          m_caret = 0;
          break;  // Caret home
        case 79:
        case 103:
          m_caret = m_text.size();  // Caret end
          break;
        case 83:  // Delete, win
          if (m_caret != m_text.size()) {
            m_text.erase(m_text.begin() + m_caret);
          }
          break;
      }
    }
  }

  const std::wstring& get_text() { return m_text; }

  void set_text(const std::wstring& text) {
    m_text = text;
    m_caret = 0;
  }

  void show_caret(bool show) { m_show_caret = show; }

 private:
  font m_font;
  sdl_surface m_caretimg;
  std::wstring m_text;
  unsigned m_caret;
  unsigned m_max;
  bool m_show_caret;
};

#endif
