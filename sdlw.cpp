#include "sdlw.hpp"
#include "resource.hpp"

#include "third_party/stb_image.h"

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>

sdl_surface load_image(std::string filename, bool alpha, bool convert) {
  FILE* image_file = std::fopen(filename.c_str(), "rb");
  if (image_file == nullptr) {
    throw std::runtime_error("couldn't open " + filename);
  }
  int width, height, depth;
  unsigned char* data =
      stbi_load_from_file(image_file, &width, &height, &depth, 0);
  std::fclose(image_file);
  if (data == 0) {
    throw std::runtime_error("sdlw error: couldn't load file " + filename +
                             " because: " + stbi_failure_reason());
  }

  // Note the order of creation and destruction here. First create an
  // SDL_Surface from the stb image data. This only references the stb image
  // data, it does not copy it. This SDL_Surface has to be released before the
  // stb image data can be free'd.
  SDL_Surface* raw =
      SDL_CreateRGBSurfaceFrom(data, width, height, depth * 8, (width * depth),
                               0xFF, 0xFF00, 0xFF0000, alpha ? 0xFF000000 : 0);
  if (raw == 0) {
    throw std::runtime_error("sdlw error: could not create surface from file " +
                             filename);
  }

  sdl_surface res;

  if (convert) {
    // Convert the raw surface to an sdl_surface that has the display pixel
    // format. We now have a copy of the image data.
    res = sdl_surface(alpha ? SDL_DisplayFormatAlpha(raw)
                            : SDL_DisplayFormat(raw));

    // Release the unconverted SDL_Surface.
    SDL_FreeSurface(raw);
  } else {
    // Just take ownership of the raw surface
    res = sdl_surface(raw);
  }

  // Free the stb image data.
  stbi_image_free(data);

  return res;
}

static std::string surface_flags_to_string(int flags) {
  std::string res;

  if (flags & SDL_SRCALPHA) res += "srcalpha,";
  if (flags & SDL_HWACCEL) res += "hwaccel,";
  if (flags & SDL_PREALLOC) res += "prealloc,";

  if (!res.empty()) res.resize(res.size() - 1);

  return res;
}

std::ostream& operator<<(std::ostream& os, const sdl_surface& surf) {
  SDL_Surface* s = surf;

  return os << "sdl_surface {"
            << " w:" << s->w << " h:" << s->h << " refc:" << s->refcount
            << " flags:" << surface_flags_to_string(s->flags) << " }";
}
