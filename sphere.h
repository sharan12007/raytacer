#ifndef SPHERE_H
#define SPHERE_H
#include "hittable.h"
#include "vec3.h"

#include <cmath>


class sphere : public hittable {

private:
	double radius;
	vec3 center;
public:

	sphere(double rad, vec3 cen) : radius(fmax(rad,0.0)), center(cen){}
	bool  hit(const Ray& r, double ray_tmin, double ray_tmax, hitRecord& rec) const override {
		const vec3 oc = r.origin() - center;
		const double a = glm::dot(r.direction(), r.direction());
		const double half_b = glm::dot(oc, r.direction());
		const double c = glm::dot(oc, oc) - radius * radius;
		const double discriminant = half_b * half_b - a * c;
		if (discriminant < 0.0 || a == 0.0)
			return false;

		const double root = std::sqrt(discriminant);
		double hitpoint = (-half_b - root) / a;
		if (hitpoint <= ray_tmin || ray_tmax <= hitpoint) {
			hitpoint = (-half_b + root) / a;
			if (hitpoint <= ray_tmin || ray_tmax <= hitpoint)
				return false;
		}

		rec.t = hitpoint;
		rec.p = r.at(rec.t);
		vec3 normal = radius > 0.0 ? (rec.p - center) / radius : vec3(0.0, 1.0, 0.0);
		rec.set_surface_normal(r,normal);
		return true;
	}
};


#endif 