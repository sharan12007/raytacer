
#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "ray.h"
#include "hittable_list.h"

#include <cmath>

class camera {
public:
    vec3 forward;
    vec3 right;
    vec3 up;
    vec3 worldup;

    vec3 origin;
    point3 position;

    double yaw = -90.0;
    double pitch = 0.0;

    double viewport_width;
    double viewport_height;
    double focal_length;

    int max_depth = 10;

    void update_camera_vectors()
    {
        vec3 newforward;

        newforward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newforward.y = sin(glm::radians(pitch));
        newforward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        forward = glm::normalize(newforward);
        right = glm::normalize(glm::cross(forward, worldup));
        up = glm::normalize(glm::cross(right, forward));
    }

    camera(double aspectratio, point3 origin)
    {
        position = origin;
        this->origin = origin;

        focal_length = 1.0;

        yaw = -90.0;
        pitch = 0.0;

        worldup = vec3(0, 1, 0);

        update_camera_vectors();

        viewport_height = 2.0;
        viewport_width = viewport_height * aspectratio;
    }

    void update_rotate(double xdelta, double ydelta)
    {
        yaw += xdelta;
        pitch += ydelta;

        pitch = glm::clamp(pitch, -89.0, 89.0);

        update_camera_vectors();
    }

    color sky_color(const Ray& r) const
    {
        vec3 unit_direction = glm::normalize(r.direction());
        double a = 0.5 * (unit_direction.y + 1.0);
        return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
    }

    color get_ray_color(const Ray& r,
        const hittable_list& world,
        int depth)
    {
        // Stop recursive bouncing
        if (depth <= 0)
            return sky_color(r);

        hitRecord hr;

        // Check whether the ray hits an object
        if (world.hit(r, 0.001, 1000000, hr))
        {
            // Generate a random bounce direction
            vec3 randomunitvector =hr.normal+ random_on_hemisphere(hr.normal);

            // Create a new ray and trace it recursively
			return 0.5 * get_ray_color(Ray(hr.p, randomunitvector), world, depth - 1);
        }

        // Ray missed everything: return sky gradient
        return sky_color(r);
    }
};

#endif
