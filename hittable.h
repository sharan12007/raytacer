#ifndef HITTABLE_H
#define	HITTABLE_H
#include "ray.h"
#include "vec3.h"
using namespace std;
class hitRecord {
public:
	point3 p;
	vec3 normal;
	double t;
	bool frontface;
	void set_surface_normal(const Ray&r,const vec3 unnormal) {
		auto dotproduct = glm::dot(r.direction(), unnormal);
		frontface = dotproduct < 0;
		normal = (frontface) ? unnormal : -unnormal;
	}
};
class hittable {
public:

	virtual ~hittable() = default;
	virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, hitRecord& rec) const = 0;
};


#endif // !HITTABLE_H
