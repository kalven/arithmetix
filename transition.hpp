#ifndef ARITHMETIX_TRANSITION_HPP
#define ARITHMETIX_TRANSITION_HPP

#include <boost/variant.hpp>

struct game_end_transition
{
    bool finish;
    int bonus, queue, specials, time;
};

struct game_start_transition
{
    unsigned int seed;
};

typedef boost::variant<void*,
                       game_end_transition,
                       game_start_transition> state_arg;

#endif
