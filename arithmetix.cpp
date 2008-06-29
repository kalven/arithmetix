#include "arithmetix.hpp"
#include "resource.hpp"
#include "button.hpp"

#include <boost/spirit/core.hpp>

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <map>

using namespace boost::spirit;

typedef std::map<std::string, sdl_surface> surface_map;
typedef std::map<std::string, sdlw_sound> sound_map;
typedef std::map<std::string, button_resource> button_map;

static surface_map cached_surfaces;
static sound_map   cached_sounds;
static button_map  button_resources;

static std::string data_dir = "data/";

void load_button_resources(button_map &, std::string filename);

sdl_surface load_image_resource(std::string name, int options)
{
    bool alpha = (options & with_alpha);
    bool convert = !(options & no_convert);
    surface_map::iterator iter = cached_surfaces.find(name);
    if(iter == cached_surfaces.end())
    {
        iter = cached_surfaces.insert(std::make_pair(name, load_image(data_dir + name, alpha, convert))).first;
    }

    return iter->second;
}

sdlw_sound load_sound_resource(std::string name)
{
    sound_map::iterator iter = cached_sounds.find(name);
    if(iter == cached_sounds.end())
    {
        // TODO: Load sounds from data/ as well
        iter = cached_sounds.insert(std::make_pair(name, load_sample(name))).first;
    }

    return iter->second;
}

void arithmetix_init_resources()
{
    load_button_resources(button_resources, data_dir + "/buttons.dat");
}

const button_resource & load_button_resource(std::string name)
{
    button_map::iterator i = button_resources.find(name);
    if(i == button_resources.end())
        throw std::runtime_error("Nonexistant button resource " + name);
    return i->second;
}

void load_button_resources(button_map & bmap, std::string filename)
{
    int def_len;
    void * def = get_resource_mem((data_dir + "buttons.dat").c_str(), &def_len);

    std::istringstream is(std::string((const char *)def, def_len));
    std::string line;

    while(getline(is, line))
    {
        std::string name, base, bpng, hpng, fpng;
        rect br, hr, fr;

        if(parse(line.c_str(),
                 lexeme_d[(+alpha_p)[assign(name)] >> !(":" >> (+alpha_p)[assign(base)])] >> "{" >>
                 // Optional components
                 !("bg:" >> lexeme_d[(+alpha_p)][assign(bpng)]) >>
                 !("bg-src:" >> int_p[assign(br.x)] >> "," >> int_p[assign(br.y)] >> "," >> int_p[assign(br.w)] >> "," >> int_p[assign(br.h)]) >>
                 !("bg-hot:" >> lexeme_d[(+alpha_p)][assign(hpng)]) >>
                 !("bg-hot-src:" >> int_p[assign(hr.x)] >> "," >> int_p[assign(hr.y)] >> "," >> int_p[assign(hr.w)] >> "," >> int_p[assign(hr.h)]) >>
                 !("fg:" >> lexeme_d[(+alpha_p)][assign(fpng)]) >>
                 !("fg-src:" >> int_p[assign(fr.x)] >> "," >> int_p[assign(fr.y)] >> "," >> int_p[assign(fr.w)] >> "," >> int_p[assign(fr.h)]) >>
                 "}" >> end_p, space_p).full)
        {
            button_resource res;

            if(!base.empty())
            {
                // If this button resource inherits from a base,
                // make sure it has been previously defined and
                // make the current resource a copy of it
                button_map::iterator basei = bmap.find(base);
                if(basei == bmap.end())
                    throw std::runtime_error("button resource " + name + " refers to nonexistant base '" + base + "'");
                res = basei->second;
            }

            // Check if the optional fields have been set and
            // overwrite the (possibly inherited) values in the
            // button resource
            if(!bpng.empty())
                res.bg.surface = load_image_resource(bpng + ".png", with_alpha);
            if(br.w * br.h > 0)
                res.bg.src = br;
            if(!hpng.empty())
                res.bg_hot.surface = load_image_resource(hpng + ".png", with_alpha);
            if(hr.w * hr.h > 0)
                res.bg_hot.src = hr;
            if(!fpng.empty())
                res.fg.surface = load_image_resource(fpng + ".png", with_alpha);
            if(fr.w * fr.h > 0)
                res.fg.src = fr;

            bmap.insert(std::make_pair(name, res));
        }
        else
        {
            throw std::runtime_error("Parse error on button def '" + line + "'");
        }
    }
}
