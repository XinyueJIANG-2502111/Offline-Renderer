#pragma once

# include "Object.hpp"
# include "Intersection.hpp"
# include "Material.hpp"


// Moller-Trumbore Algorithm
bool RayTriangleIntersect(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
	const Vector3f& orig, const Vector3f& dir, float& t_near, float& u, float& v)
{
	Vector3f e1 = v1 - v0;
	Vector3f e2 = v2 - v0;	
	Vector3f s1 = CrossProduct(dir, e2);
	// s = o - p0
	// s2 = s × e1
	
	// s1 · e1
	float det = DotProduct(s1, e1);
	if (fabs(det) < 1e-6) { return false; }

	// b1（u）
	Vector3f s = orig - v0;
	u = DotProduct(s1, s);
	if ((u < 0) || (u > det)) { return false; }

	// b2（v）
	Vector3f s2 = CrossProduct(s, e1);
	v = DotProduct(s2, dir);
	if ((v < 0) || ((u + v) > det)) { return false; }

	// 1 / (s1 · e1)
	float inv_det = 1.f / det;

	// t，b1，b2
	t_near = DotProduct(s2, e2) * inv_det;
	if (t_near < 1e-6) { return false; }

	u *= inv_det;
	v *= inv_det;

	return true; 
}

class Triangle : public Object
{
public:
	Vector3f v0, v1, v2;	   // vertex
	Vector3f e1, e2;		   // edge
	Vector3f tex0, tex1, tex2; // texture coords
	Vector3f normal;
	float area;
	Material* m;

	Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Material* _m = nullptr)
		: v0(_v0), v1(_v1), v2(_v2), m(_m)
	{
		e1 = v1 - v0;
		e2 = v2 - v0;
		normal = Normalize(CrossProduct(e1, e2));
		area = CrossProduct(e1, e2).magnitude() * 0.5f;
	}

	inline bool Intersect(const Ray& ray) { return true; }
	inline bool Intersect(const Ray& ray, float& t_near, uint32_t& index) const { return false; }
	
	inline Intersection GetIntersection(Ray ray) {
		Intersection inter;

		if (DotProduct(ray.direction, normal) > 0) { return inter; }

		// Use barycentric coordinates to test the hit against the triangle edges.
		double b1, b2, t_tmp = 0;
		Vector3f s1 = CrossProduct(ray.direction, e2);
		double det = DotProduct(s1, e1);
		if (fabs(det) < 1e-6) { return inter; }

		Vector3f s = ray.origin - v0;
		b1 = DotProduct(s1, s);
		if ((b1 < 0) || (b1 > det)) { return inter; }

		Vector3f s2 = CrossProduct(s, e1);
		b2 = DotProduct(s2, ray.direction);
		if ((b2 < 0) || ((b1 + b2) > det)) { return inter; }

		t_tmp = DotProduct(s2, e2) / det;
		if (t_tmp < 1e-6) { return inter; }

		inter.happened = true;
		inter.coords = ray(t_tmp);
		inter.normal = normal;
		inter.distance = t_tmp;
		inter.obj = this;
		inter.m = m;

		return inter;
	}

	inline void GetSurfaceProperties(const Vector3f& P, const Vector3f& I,
		const uint32_t& index, const Vector2f& uv, Vector3f& N, Vector2f& st) const 
	{
		N = normal;
	}

	inline Vector3f EvalDiffuseColor(const Vector2f&) const { return Vector3f(0.5f, 0.5f, 0.5f); }

	inline Bounds3 GetBounds() { return Union(Bounds3(v0, v1), v2); }
	
	inline void Sample(Intersection& pos, float& pdf) {
		// The square root makes the barycentric sample uniform over the surface.
		float x = std::sqrt(get_random_float());
		float y = get_random_float();

		pos.coords = v0 * (1.0f - x) + v1 * (x * (1.0f - y)) + v2 * (x * y);
		pos.normal = this->normal;
		pdf = 1.0f / area;
	}

	inline float GetArea() { return area; }
	inline bool HasEmit() { return m->HasEmission(); }
};