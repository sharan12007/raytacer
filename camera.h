#ifndef CAMERA_H
#define CAMERA_H
#include "vec3.h"
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
	void update_camera_vectors() {
		vec3 newforward;
		newforward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		newforward.y = sin(glm::radians(pitch));
		newforward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		forward = glm::normalize(newforward);
		right = glm::normalize(glm::cross(newforward, worldup));
		up = glm::normalize(glm::cross(right, newforward));
	}

	camera(double aspectratio, point3 origin)
	{
		position = origin;
		focal_length = 1.0;

		yaw = -90.0;
		pitch = 0.0;

		worldup = vec3(0, 1, 0);
		update_camera_vectors();
		position = point3(0, 0, 0);

		forward = vec3(0, 0, -1);
		right = vec3(1, 0, 0);
		up = vec3(0, 1, 0);
		viewport_width = 2.0;
		viewport_height = viewport_width / aspectratio;
	}
	void update_rotate(double xdelta,double ydelta) {
		yaw += xdelta;
		pitch += ydelta;

		pitch=glm::clamp(pitch, -89.0, 89.0);
		update_camera_vectors();
	}

};
#endif // !CAMERA_H
