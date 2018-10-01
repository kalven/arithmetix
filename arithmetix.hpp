#ifndef ARITHMETIX_ARITHMETIX_HPP
#define ARITHMETIX_ARITHMETIX_HPP

#include "sdlw.hpp"

struct button_resource;

enum { no_options = 0, with_alpha = 1, no_convert = 2 };

sdl_surface load_image_resource(std::string name, int options = no_options);

// Call this after screen has been created since it will load graphics
void arithmetix_init_resources();

const button_resource& load_button_resource(std::string name);

#endif
