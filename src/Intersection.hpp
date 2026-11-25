#pragma once
# include <limits>
# include "Vector.hpp"
# include "Material.hpp"

class Object;

struct Intersection 
{
	bool happened;	

	Vector3f coords;
	Vector3f t_coords; 
	Vector3f normal;   
	Vector3f emit;	

	double distance;

	Object* obj;	
	Material* m;	

	// ctor
	Intersection() : happened(false), 
		coords(vec3f()), t_coords(vec3f()), normal(vec3f()),
		distance(std::numeric_limits<double>::max()),
		obj(nullptr), m(nullptr)
	{}
};