#ifndef SPHERE_H
#define SPHERE_H
#include "hittable.h"
#include "vec3.h"


class sphere : public hittable {

private:
	double radius;
	vec3 center;
public:

	sphere(double rad, vec3 cen) : radius(fmax(rad,0.0)), center(cen){}
	bool  hit(const Ray& r, double ray_tmin, double ray_tmax, hitRecord& rec) const override {
		auto a = glm::dot(r.direction(), r.direction());
		auto b = -2.0 * glm::dot(r.direction(), center - r.origin());
		auto c = glm::dot(center - r.origin(), center - r.origin()) - (radius * radius);
		auto discriminant = b * b - 4 * a * c;
		if (discriminant < 0) return false;
		auto root1 = (-b + sqrt(discriminant)) / (2.0 * a);
		auto root2 = (-b - sqrt(discriminant)) / (2.0 * a);
		if (root1 < 0 && root2 < 0) return false;
		double hitpoint = DBL_MAX;
		if (root1 > 0) {
			hitpoint = min(hitpoint, root1);
		}
		if (root2 > 0) {
			hitpoint = min(hitpoint, root2);
		}
		rec.t = hitpoint;
		rec.p = r.at(rec.t);
		vec3 normal = (rec.p - center) / 0.5;
		rec.set_surface_normal(r,normal);
		return true;
	}
};


#endif 