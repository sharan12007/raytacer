
#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {
private:
    vec3 orig;
    vec3 dir;

public:
    Ray(const vec3& orig, const vec3& dir)
        : orig(orig), dir(dir) {
    }

    vec3 origin() const {
        return orig;
    }

    vec3 direction() const {
        return dir;
    }

    vec3 at(double t) const {
        return orig + t * dir;
    }
};

#endif
