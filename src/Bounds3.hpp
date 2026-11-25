#pragma once
# include <limits>
# include <array>

# include "Ray.hpp"
# include "Vector.hpp"

// axis-aligned bounding box, AABB
class Bounds3 {
public:
	Vector3f p_min, p_max;

	// ctor
	Bounds3() {
		constexpr float min_num = std::numeric_limits<float>::lowest();
		constexpr float max_num = std::numeric_limits<float>::max();

		p_max = Vector3f(min_num, min_num, min_num);
		p_min = Vector3f(max_num, max_num, max_num);
	}
	Bounds3(const Vector3f& p) : p_min(p), p_max(p) {}
	Bounds3(const Vector3f& p1, const Vector3f& p2) {
		p_min = Vector3f(fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z));
		p_max = Vector3f(fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z));
	}

	Vector3f Diagonal() const { return Vector3f(p_max - p_min); }

	int MaxExtent() const {
		Vector3f diag = Diagonal();
		if ((diag.x > diag.y) && (diag.x > diag.z)) { return 0; }
		else if (diag.y > diag.z) { return 1; }					 
		else { return 2; }										 
	}

	double SurfaceArea() const {
		Vector3f diag = Diagonal();
		return (2 * (diag.x * diag.y + diag.x * diag.z + diag.y * diag.z));
	}

	Vector3f Centroid() const { return (0.5 * p_min + 0.5 * p_max); }

	Bounds3 Intersect(const Bounds3& b) {
		return Bounds3(Vector3f(fmax(p_min.x, b.p_min.x), fmax(p_min.y, b.p_min.y), fmax(p_min.z, b.p_min.z)),
					   Vector3f(fmin(p_max.x, b.p_max.x), fmin(p_max.y, b.p_max.y), fmin(p_max.z, b.p_max.z)));
	}

	Vector3f Offset(const Vector3f& p) {
		Vector3f o = p - p_min;
		if (p_max.x > p_min.x) { o.x /= (p_max.x - p_min.x); }
		if (p_max.y > p_min.y) { o.y /= (p_max.y - p_min.y); }
		if (p_max.z > p_min.z) { o.z /= (p_max.z - p_min.z); }
		return o;
	}

	bool Overlaps(const Bounds3& b1, const Bounds3& b2) {
		bool x = (b1.p_max.x >= b2.p_min.x) && (b1.p_min.x <= b2.p_max.x);
		bool y = (b1.p_max.y >= b2.p_min.y) && (b1.p_min.y <= b2.p_max.y);
		bool z = (b1.p_max.z >= b2.p_min.z) && (b1.p_min.z <= b2.p_max.z);
		return (x && y && z);
	}

	bool Inside(const Vector3f& p, const Bounds3& b) {
		return (
			((p.x >= b.p_min.x) && (p.x <= b.p_max.x)) &&
			((p.y >= b.p_min.y) && (p.y <= b.p_max.y)) &&
			((p.z >= b.p_min.z) && (p.z <= b.p_max.z))
			);
	}

	inline const Vector3f& operator[](int i) const { return (i == 0) ? p_min : p_max; }

	inline bool IntersectP(const Ray& ray, const Vector3f& inv_dir, 
						   const std::array<int, 3>& dir_is_neg) const;
	
	inline bool IntersectP(const Ray& ray, const std::array<int, 3>& dir_is_neg) const;
	

};

inline bool Bounds3::IntersectP(const Ray& ray, const Vector3f& inv_dir,
	const std::array<int, 3>& dir_is_neg) const
{
	// dir_is_neg = [int(x > 0),int(y > 0),int(z > 0)]
	
	float x_t_min = (p_min.x - ray.origin.x) * inv_dir.x;
	float x_t_max = (p_max.x - ray.origin.x) * inv_dir.x;

	float y_t_min = (p_min.y - ray.origin.y) * inv_dir.y;
	float y_t_max = (p_max.y - ray.origin.y) * inv_dir.y;

	float z_t_min = (p_min.z - ray.origin.z) * inv_dir.z;
	float z_t_max = (p_max.z - ray.origin.z) * inv_dir.z;

	if (!dir_is_neg[0]) std::swap(x_t_min, x_t_max);
	if (!dir_is_neg[1]) std::swap(y_t_min, y_t_max);
	if (!dir_is_neg[2]) std::swap(z_t_min, z_t_max);

	float t_in = std::max(x_t_min, std::max(y_t_min, z_t_min));
	float t_out = std::min(x_t_max, std::min(y_t_max, z_t_max));

	return ((t_in <= t_out) && (t_out >= 0));
}

inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2) {
	Bounds3 ret;
	ret.p_min = Vector3f::Min(b1.p_min, b2.p_min);
	ret.p_max = Vector3f::Max(b1.p_max, b2.p_max);
	return ret;
}

inline Bounds3 Union(const Bounds3& b, const Vector3f& p) {
	Bounds3 ret;
	ret.p_min = Vector3f::Min(b.p_min, p);
	ret.p_max = Vector3f::Max(b.p_max, p);
	return ret;
}