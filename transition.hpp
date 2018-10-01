#ifndef ARITHMETIX_TRANSITION_HPP
#define ARITHMETIX_TRANSITION_HPP

#include <variant>

struct game_end_transition {
  bool finish;
  int bonus, queue, specials, time;
};

struct game_start_transition {
  unsigned int seed;
};

using state_arg =
    std::variant<std::monostate, game_end_transition, game_start_transition>;

#endif
