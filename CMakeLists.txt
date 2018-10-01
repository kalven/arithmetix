cmake_minimum_required(VERSION 3.0)

find_package(SDL REQUIRED)

add_executable(arithmetix
  "arithmetix.cpp"
  "arithmetix.hpp"
  "board.hpp"
  "button.cpp"
  "button.hpp"
  "credits.cpp"
  "credits.hpp"
  "edit.hpp"
  "event.cpp"
  "event.hpp"
  "font.cpp"
  "font.hpp"
  "game.cpp"
  "game.hpp"
  "hiscore.cpp"
  "hiscore.hpp"
  "main.cpp"
  "queue.hpp"
  "resource.cpp"
  "resource.hpp"
  "sdlw.cpp"
  "sdlw.hpp"
  "state.hpp"
  "status.hpp"
  "stb.cpp"
  "title.cpp"
  "title.hpp"
  "transition.hpp")

target_compile_features(arithmetix PUBLIC cxx_std_17)
target_include_directories(arithmetix PRIVATE ${SDL_INCLUDE_DIR})
target_link_libraries(arithmetix ${SDL_LIBRARY})
