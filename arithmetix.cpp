#include "arithmetix.hpp"
#include "button.hpp"
#include "resource.hpp"

#include <cstring>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

using surface_map = std::map<std::string, sdl_surface>;
using button_map = std::map<std::string, button_resource>;

static surface_map cached_surfaces;
static button_map  button_resources;

static std::string data_dir = "data/";

void load_button_resources(button_map&, std::string filename);

sdl_surface load_image_resource(std::string name, int options) {
  bool alpha = (options & with_alpha);
  bool convert = !(options & no_convert);
  auto it = cached_surfaces.find(name);
  if (it == cached_surfaces.end()) {
    it = cached_surfaces
             .emplace(name, load_image(data_dir + name, alpha, convert))
             .first;
  }

  return it->second;
}

void arithmetix_init_resources() {
  load_button_resources(button_resources, data_dir + "/buttons.dat");
}

const button_resource& load_button_resource(std::string name) {
  auto it = button_resources.find(name);
  if (it == button_resources.end()) {
    throw std::runtime_error("Nonexistant button resource " + name);
  }
  return it->second;
}

void load_button_resources(button_map& bmap, std::string filename) {
  int def_len;
  void* def = get_resource_mem((data_dir + "buttons.dat").c_str(), &def_len);

  std::istringstream is(std::string((const char*)def, def_len));
  std::string line;

  while (getline(is, line)) {
    if (line.empty()) {
      continue;
    }

    std::string name, base, bpng, hpng, fpng;
    rect br, hr, fr;

    char name_base[16];
    char def[240];

    if (std::sscanf(&line[0], "%s { %[^}] }", name_base, def) != 2) {
      throw std::runtime_error("Parse failed: " + line);
    }

    if (char* basep = std::strchr(name_base, ':')) {
      *basep = 0;
      base = basep + 1;
    }

    name = name_base;

    char* p = def;
    while (char* e = std::strchr(p, ' ')) {
      *e = 0;

      char source[32];

      if (std::sscanf(p, "bg:%s", source) == 1) {
        bpng = source;
      } else if (std::sscanf(p, "bg-src:%hd,%hd,%hu,%hu", &br.x, &br.y, &br.w,
                             &br.h) == 4) {
      } else if (std::sscanf(p, "bg-hot:%s", source) == 1) {
        hpng = source;
      } else if (std::sscanf(p, "bg-hot-src:%hd,%hd,%hu,%hu", &hr.x, &hr.y,
                             &hr.w, &hr.h) == 4) {
      } else if (std::sscanf(p, "fg:%s", source) == 1) {
        fpng = source;
      } else if (std::sscanf(p, "fg-src:%hd,%hd,%hu,%hu", &fr.x, &fr.y, &fr.w,
                             &fr.h) == 4) {
      } else {
        throw std::runtime_error("Parse failed: " + line);
      }

      p = e + 1;
    }

    button_resource res;

    if (!base.empty()) {
      // If this button resource inherits from a base, make sure it has been
      // previously defined and make the current resource a copy of it
      button_map::iterator basei = bmap.find(base);
      if (basei == bmap.end()) {
        throw std::runtime_error("button resource " + name +
                                 " refers to nonexistant base '" + base + "'");
      }
      res = basei->second;
    }

    // Check if the optional fields have been set and overwrite the (possibly
    // inherited) values in the button resource
    if (!bpng.empty()) {
      res.bg.surface = load_image_resource(bpng + ".png", with_alpha);
    }
    if (br.w * br.h > 0) {
      res.bg.src = br;
    }
    if (!hpng.empty()) {
      res.bg_hot.surface = load_image_resource(hpng + ".png", with_alpha);
    }
    if (hr.w * hr.h > 0) {
      res.bg_hot.src = hr;
    }
    if (!fpng.empty()) {
      res.fg.surface = load_image_resource(fpng + ".png", with_alpha);
    }
    if (fr.w * fr.h > 0) {
      res.fg.src = fr;
    }

    bmap.insert(std::make_pair(name, res));
  }
}
