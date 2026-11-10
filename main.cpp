#include <iostream>
#include "colour.h"
#include "vec3.h"
#include "ray.h"
#include <random>
#include "sphere.h"
#include "camera.h"
#include "hittable_list.h"
using namespace std;
double get_safe_rand() {
	static std::random_device rd;
	static std::mt19937 gen(rd());
	return std::generate_canonical<double, 10>(gen);
}


color get_ray_color(const Ray& r,const hittable_list&world) {
	
	hitRecord hr;
	
	if (world.hit(r,0.001,100,hr)){
		auto SurfaceNormal = hr.normal;
		return  0.5 * color(SurfaceNormal.x + 1, SurfaceNormal.y + 1, SurfaceNormal.z + 1);
	}
	auto unit_direction = glm::normalize(r.direction());
	auto blend = 0.5 * (unit_direction.y + 1.0);
	return color(0.0, 0.0, 1.0) * (1.0 - blend)
		+ color(0.0, 1.0, 0.0) * blend;
}

vector<unsigned char> render_image(int IMAGE_WIDTH, int IMAGE_HEIGHT, camera & cam) {
	
	vector<unsigned char> pixels;
	double aspect_ratio = static_cast<double>(IMAGE_WIDTH) / IMAGE_HEIGHT;
	int SAMPLES_PER_PIEXL = 1;
	

	vec3 origin = cam.position;
	vec3 horizontal=cam.right* cam.viewport_width;
	vec3 vertical= -cam.viewport_height *cam.up;
	vec3 focal_length = cam.focal_length * cam.forward;

	vec3 x_delta = horizontal / double(IMAGE_WIDTH);
	vec3 y_delta = vertical / double(IMAGE_HEIGHT);
	hittable_list world;
	world.add(make_shared<sphere>(0.5, point3(0, 0, -4)));

	vec3 upper_left_corner = origin + focal_length - (horizontal *0.5) - (vertical *0.5);
	auto center00 = upper_left_corner + x_delta / double(2.0) + y_delta / double(2.0);
	for (int i = 0;i < IMAGE_HEIGHT;i++) {
		clog << "\rScanlines remaining: " << IMAGE_HEIGHT - i << flush;
		for (int j = 0;j < IMAGE_WIDTH;j++) {
			//anti-aliasing
			color pixel_color(0, 0, 0);
			for (int s = 0;s < SAMPLES_PER_PIEXL;s++) {
				auto pixel_center = center00 + x_delta * (j+ get_safe_rand()-0.5) + y_delta * ( i+ get_safe_rand()-0.5);
				auto direction = pixel_center - origin;
				Ray r(origin, direction);
				pixel_color += get_ray_color(r,world);
			}
			pixel_color /= SAMPLES_PER_PIEXL;
			vector<unsigned char> pixel = convert_colour(pixel_color);
			for (auto px : pixel) {
				pixels.push_back(px);
			}
		}
	}
	clog << "Done.\n";
	return pixels;
}