#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <godot_cpp/core/defs.hpp>

real_t random(real_t min, real_t max) {
    std::random_device rd;
    std::uniform_real_distribution<real_t> dist(min, max);

    std::mt19937 mt(rd());

    return dist(mt);
}

#endif // RANDOM_H