#ifndef VEC3_H
#define VEC3_H

#include <glm/glm.hpp>
#include <random>

using vec3 = glm::dvec3;
using point3 = glm::dvec3;
using color = glm::dvec3;

inline double get_safe_rand()
{
    thread_local std::mt19937 gen(std::random_device{}());

    return std::generate_canonical<double, 10>(gen);
}
inline double length_squared(const vec3&e) 
{
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
}
inline double random_double(double min, double max) {
    return min + (max - min) * get_safe_rand();
}

inline vec3 random_vec3() 
{
    return vec3(
        get_safe_rand(),
        get_safe_rand(),
        get_safe_rand()
    );
}

inline vec3 random_vec3(double min, double max)
{
    return vec3(
        random_double(min, max),
        random_double(min, max),
        random_double(min, max)
    );
}
inline vec3 random_unit_vector(){
    while (true)
    {
        vec3 p = random_vec3(-1.0, 1.0);
        double len_sq = length_squared(p);
        if (1e-160 < len_sq && len_sq <= 1.0)
            return p / sqrt(len_sq);
    }

}

inline vec3 random_on_hemisphere(const vec3& normal)
{
    vec3 on_unit_sphere = random_unit_vector();

    if (dot(on_unit_sphere, normal) > 0.0)
        return on_unit_sphere;

    return -on_unit_sphere;
}

#endif // !VEC3_H
