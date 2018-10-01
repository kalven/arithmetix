#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <vector>

#include "arithmetix.hpp"
#include "sdlw.hpp"
#include "state.hpp"

#include <SDL/SDL_main.h>

// States
#include "credits.hpp"
#include "game.hpp"
#include "hiscore.hpp"
#include "title.hpp"

state_machine sm;

int main(int argc, char* argv[]) {
  SDL_Event event;
  bool done = false;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Oh noes!\n";
    return 0;
  }

  std::atexit(SDL_Quit);

  // sdlw_start_audio();

  try {
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    SDL_WM_SetCaption("Arithmetix!", "gonigogo");

    sdl_surface screen(SDL_SetVideoMode(432, 434, 32, 0));

    arithmetix_init_resources();

    sm.add_state("game", std::make_unique<game>(sm));
    sm.add_state("hiscore", std::make_unique<hiscore>(sm));
    sm.add_state("title", std::make_unique<title>(sm));
    sm.add_state("credits", std::make_unique<credits>(sm));

    sm.set_next_state("title");

    while (!done) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          done = true;
        }
        sm.send_event(event);
      }

      sm.draw(screen);

      SDL_Flip(screen);
      SDL_Delay(15);
    }
  } catch (const std::exception& e) {
    std::cerr << "Caught top-level exception: " << e.what() << std::endl;
  }

  return 0;
}
