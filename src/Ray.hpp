#pragma once

# include <limits>
# include "Vector.hpp"

struct Ray {
	Vector3f origin;
	Vector3f direction, inv_direction;
	
	double t;
	double t_min, t_max;

	// ctor
	Ray(const Vector3f& o, const Vector3f& d, const double _t = 0.0) :
		origin(o), direction(d), t(_t) 
	{
		inv_direction = Vector3f(1.f / d.x, 1.f / d.y, 1.f / d.z);

		t_min = 0.0;
		t_max = std::numeric_limits<double>::max();
	}

	Vector3f operator ()(double t) const { return origin + t * direction; }

	// test
	friend std::ostream& operator << (std::ostream& output, const Ray& ray) {
		output << "[origin = " << ray.origin << " ,direction = " << ray.direction 
			<< " ,time = " << ray.t << "]\n";
		return output;
	}
};