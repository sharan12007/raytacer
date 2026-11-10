#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>
using namespace std;
using color = vec3;
 vector<unsigned char> convert_colour(const color& colours){
	 vector<unsigned char> result;
	 result.push_back(static_cast<unsigned char>(255.999 * colours.x));
	 result.push_back(static_cast<unsigned char>(255.999 * colours.y));
	 result.push_back(static_cast<unsigned char>(255.999 * colours.z));
	 return result;
}
#endif