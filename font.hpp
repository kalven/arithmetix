#ifndef ARITHMETIX_FONT_HPP
#define ARITHMETIX_FONT_HPP

#include "arithmetix.hpp"

#include <algorithm>
#include <fstream>
#include <map>
#include <string>

class font {
 public:
  font(const std::string& name)
      : m_bmp(load_image_resource(name + ".png", with_alpha)) {
    load_def(name);
  }

  void render(sdl_surface& screen, int x, int y, const std::wstring& text) {
    SDL_Rect dest = {Sint16(x), Sint16(y)};
    for (unsigned i = 0; i != text.size(); ++i) {
      std::map<wchar_t, char_info>::iterator ci = m_chars.find(text[i]);
      if (ci != m_chars.end()) {
        char_info& c = ci->second;
        if (i != 0) {
          // Kerning
          auto ki = m_kern.find(std::make_pair(text[i], text[i - 1]));
          if (ki != m_kern.end()) {
            dest.x += ki->second;
          }
        }
        SDL_Rect glyph_dest = {Sint16(dest.x + c.xoffset),
                               Sint16(dest.y + c.yoffset)};
        SDL_BlitSurface(m_bmp, &c.src, screen, &glyph_dest);
        dest.x += c.xadvance;
      }
    }
  }

  int get_width(const std::wstring& text, unsigned chars = unsigned(-1)) {
    chars = std::min(static_cast<unsigned>(text.size()), chars);

    int width = 0;

    for (unsigned i = 0; i != chars; ++i) {
      std::map<wchar_t, char_info>::iterator ci = m_chars.find(text[i]);
      if (ci != m_chars.end()) {
        char_info& c = ci->second;
        if (i != 0) {
          std::map<kern_pair, int>::iterator ki =
              m_kern.find(std::make_pair(text[i], text[i - 1]));
          if (ki != m_kern.end()) width += ki->second;
        }

        width += c.xadvance;
      }
    }

    return width;
  }

  bool has_char(wchar_t c) { return m_chars.count(c) != 0; }

 private:
  void load_def(const std::string& name);

  sdl_surface m_bmp;

  using kern_pair = std::pair<wchar_t, wchar_t>;
  struct char_info {
    SDL_Rect src;
    int xoffset, yoffset;
    int xadvance;
  };

  std::map<wchar_t, char_info> m_chars;
  std::map<kern_pair, int> m_kern;
};

#endif
