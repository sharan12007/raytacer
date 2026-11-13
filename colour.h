#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <cmath>
#include <algorithm>
#include <vector>

using color = vec3;

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0)
        return std::sqrt(linear_component);
    return 0;
}

inline std::vector<unsigned char> convert_colour(const color& colours) {
    auto r = linear_to_gamma(colours.x);
    auto g = linear_to_gamma(colours.y);
    auto b = linear_to_gamma(colours.z);

    std::vector<unsigned char> result;
    result.push_back(static_cast<unsigned char>(256 * std::clamp(r, 0.0, 0.999)));
    result.push_back(static_cast<unsigned char>(256 * std::clamp(g, 0.0, 0.999)));
    result.push_back(static_cast<unsigned char>(256 * std::clamp(b, 0.0, 0.999)));
    return result;
}

#endif
