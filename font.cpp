#include "font.hpp"
#include "resource.hpp"

#include <boost/spirit/core.hpp>

#include <stdio.h>
#include <string.h>

using namespace boost::spirit;

void font::load_def(const std::string & name)
{
    m_chars.clear();
    m_kern.clear();

    FILE * def = get_resource_file(("data/" + name + ".fnt").c_str());

    char line[120];

    while(true)
    {
        fgets(line, sizeof(line), def);
        // TODO: Read the char count instead.
        if(line == 0 || strncmp("END", line, 3) == 0)
            break;

        {
            wchar_t char_id;
            char_info i;

            if(parse(line,
                     "char id=" >> int_p[assign(char_id)] >>
                     "x=" >> int_p[assign(i.src.x)] >>
                     "y=" >> int_p[assign(i.src.y)] >>
                     "width=" >> int_p[assign(i.src.w)] >>
                     "height=" >> int_p[assign(i.src.h)] >>
                     "xoffset=" >> int_p[assign(i.xoffset)] >>
                     "yoffset=" >> int_p[assign(i.yoffset)] >>
                     "xadvance=" >> int_p[assign(i.xadvance)] >>
                    *anychar_p >> end_p, space_p).full)
            {
                m_chars[char_id] = i;
                continue;
            }
        }

        // Try parsing kerning info
        {
            kern_pair kp;
            int amount;
            if(parse(line,
                     "kerning first=" >> int_p[assign(kp.first)] >>
                     "second=" >> int_p[assign(kp.second)] >>
                     "amount=" >> int_p[assign(amount)] >>
                    *anychar_p >> end_p, space_p).full)
            {
                m_kern[kp] = amount;
            }
        }
    }
}
