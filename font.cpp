#include "font.hpp"
#include "resource.hpp"

#include <stdio.h>
#include <string.h>

void font::load_def(const std::string& name) {
  m_chars.clear();
  m_kern.clear();

  FILE* def = get_resource_file(("data/" + name + ".fnt").c_str());

  char line[250];

  while (true) {
    fgets(line, sizeof(line), def);
    // TODO: Read the char count instead.
    if (line == 0 || strncmp("END", line, 3) == 0) break;

    {
      int char_id;
      char_info i;

      if (std::sscanf(line,
                      "char id=%d x=%hd y=%hd width=%hd height=%hd "
                      "xoffset=%d yoffset=%d xadvance=%d",
                      &char_id, &i.src.x, &i.src.y, &i.src.w, &i.src.h,
                      &i.xoffset, &i.yoffset, &i.xadvance) == 8) {
        m_chars[char_id] = i;
        continue;
      }
    }

    // Try parsing kerning info
    // {
    //     kern_pair kp;
    //     int amount;
    //     if(parse(line,
    //              "kerning first=" >> int_p[assign(kp.first)] >>
    //              "second=" >> int_p[assign(kp.second)] >>
    //              "amount=" >> int_p[assign(amount)] >>
    //             *anychar_p >> end_p, space_p).full)
    //     {
    //         m_kern[kp] = amount;
    //     }
    // }
  }
}
