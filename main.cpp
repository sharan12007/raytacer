#include <iostream>
#include <vector>
#include <random>
#include <atomic>
#include <thread>
#include <algorithm>
#include "colour.h"
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "camera.h"
#include "hittable_list.h"

using namespace std;

vector<unsigned char> render_image(int IMAGE_WIDTH, int IMAGE_HEIGHT, camera& cam) {
    vector<unsigned char> pixels(static_cast<size_t>(IMAGE_WIDTH) * IMAGE_HEIGHT * 3);

    int SAMPLES_PER_PIXEL = 100;

    vec3 origin = cam.position;
    vec3 horizontal = cam.right * cam.viewport_width;
    vec3 vertical = cam.viewport_height * cam.up; // POSITIVE: maps row 0 to bottom-left for OpenGL
    vec3 focal_length = cam.focal_length * cam.forward;

    vec3 x_delta = horizontal / double(IMAGE_WIDTH);
    vec3 y_delta = vertical / double(IMAGE_HEIGHT);

    hittable_list world;
    world.add(make_shared<sphere>(0.5, point3(0, 0, -1)));
    world.add(make_shared<sphere>(100.0, point3(0, -100.5, -1)));

    vec3 lower_left_corner = origin + focal_length - (horizontal * 0.5) - (vertical * 0.5);
    auto center00 = lower_left_corner + (x_delta * 0.5) + (y_delta * 0.5);

    atomic<int> next_row{ 0 };
    const unsigned int hardware_threads = thread::hardware_concurrency();
    const unsigned int available_workers = hardware_threads > 1
        ? hardware_threads - 1
        : 1;
    const unsigned int worker_count = min(
        available_workers,
        static_cast<unsigned int>(max(1, IMAGE_HEIGHT)));

    auto render_rows = [&]() {
        while (true) {
            const int i = next_row.fetch_add(1, memory_order_relaxed);
            if (i >= IMAGE_HEIGHT)
                break;

            for (int j = 0; j < IMAGE_WIDTH; j++) {
                color pixel_color(0, 0, 0);
                for (int s = 0; s < SAMPLES_PER_PIXEL; s++) {
                    auto pixel_center = center00 + x_delta * (j + get_safe_rand() - 0.5) + y_delta * (i + get_safe_rand() - 0.5);
                    auto direction = pixel_center - origin;
                    Ray r(origin, direction);
                    pixel_color += cam.get_ray_color(r, world, cam.max_depth);
                }
                pixel_color /= SAMPLES_PER_PIXEL;
                vector<unsigned char> pixel = convert_colour(pixel_color);
                const size_t pixel_offset = (static_cast<size_t>(i) * IMAGE_WIDTH + j) * 3;
                pixels[pixel_offset] = pixel[0];
                pixels[pixel_offset + 1] = pixel[1];
                pixels[pixel_offset + 2] = pixel[2];
            }
        }
    };

    vector<thread> workers;
    workers.reserve(worker_count);
    for (unsigned int worker = 0; worker < worker_count; worker++)
        workers.emplace_back(render_rows);

    for (auto& worker : workers)
        worker.join();

    clog << "Rendered with " << worker_count << " CPU workers.\n";
    return pixels;
}

