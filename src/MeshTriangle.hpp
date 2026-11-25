#pragma once
# include <array>
# include <stdexcept>

# include "OBJ_Loader.hpp"
# include "Triangle.hpp"
# include "BVH.hpp"

class MeshTriangle : public Object
{
public:
	Bounds3 bounding_box;
	uint32_t n_triangles;
	std::unique_ptr<Vector3f[]> vertices;
	std::unique_ptr<uint32_t[]> vertex_index;
	std::unique_ptr<Vector2f[]> st_coords;
	std::vector<Triangle> triangles;

	float area;
	BVHAccel* bvh;
	Material* m;

	MeshTriangle(const std::string& filename, Material* mat) : m(mat), area(0)
	{
		objl::Loader loader;
		if (!loader.LoadFile(filename) || loader.LoadedMeshes.size() != 1) {
			throw std::runtime_error("Expected one mesh in " + filename);
		}
		const auto& mesh = loader.LoadedMeshes.front();
		if (mesh.Vertices.empty() || mesh.Vertices.size() % 3 != 0) {
			throw std::runtime_error("Expected triangulated geometry in " + filename);
		}

		Vector3f min_vert = Vector3f{ std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity(),
									 std::numeric_limits<float>::infinity() };
		Vector3f max_vert = Vector3f{ -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity(),
									 -std::numeric_limits<float>::infinity() };
		for (int i = 0; i < mesh.Vertices.size(); i += 3) {
			std::array<Vector3f, 3> face_vertices;

			for (int j = 0; j < 3; j++) {
				auto vert = Vector3f(mesh.Vertices[i + j].Position.X,
									 mesh.Vertices[i + j].Position.Y,
									 mesh.Vertices[i + j].Position.Z);
				face_vertices[j] = vert;

				min_vert = Vector3f(std::min(min_vert.x, vert.x),
									std::min(min_vert.y, vert.y),
									std::min(min_vert.z, vert.z));
				max_vert = Vector3f(std::max(max_vert.x, vert.x),
									std::max(max_vert.y, vert.y),
									std::max(max_vert.z, vert.z));
			}

			triangles.emplace_back(face_vertices[0], face_vertices[1],
				face_vertices[2], mat);
		}

		bounding_box = Bounds3(min_vert, max_vert);

		// Build a local BVH once triangle storage is stable.
		std::vector<Object*> ptrs;
		for (auto& tri : triangles) {
			ptrs.push_back(&tri);
			area += tri.area;
		}
		bvh = new BVHAccel(ptrs);
		n_triangles = triangles.size();
	}

	bool Intersect(const Ray& ray) { return true; }
	bool Intersect(const Ray& ray, float& t_near, uint32_t& index) const {
		bool intersect = false;
		for (uint32_t k = 0; k < n_triangles; ++k) {
			const Vector3f& v0 = vertices[vertex_index[k * 3]];
			const Vector3f& v1 = vertices[vertex_index[k * 3 + 1]];
			const Vector3f& v2 = vertices[vertex_index[k * 3 + 2]];
			float t, u, v;
			if (RayTriangleIntersect(v0, v1, v2, ray.origin, ray.direction, t,
				u, v) &&
				t < t_near) {
				t_near = t;
				index = k;
				intersect |= true;
			}
		}

		return intersect;
	}

	Intersection GetIntersection(Ray ray) {
		Intersection inter;
		if (bvh) { inter = bvh->Intersect(ray); }

		return inter;
	}

	void GetSurfaceProperties(const Vector3f& P, const Vector3f& I,
		const uint32_t& index, const Vector2f& uv, Vector3f& N, Vector2f& st) const {
		const Vector3f& v0 = vertices[vertex_index[index * 3]];
		const Vector3f& v1 = vertices[vertex_index[index * 3 + 1]];
		const Vector3f& v2 = vertices[vertex_index[index * 3 + 2]];
		Vector3f e0 = Normalize(v1 - v0);
		Vector3f e1 = Normalize(v2 - v0);
		N = Normalize(CrossProduct(e0, e1));
		const Vector2f& st0 = st_coords[vertex_index[index * 3]];
		const Vector2f& st1 = st_coords[vertex_index[index * 3 + 1]];
		const Vector2f& st2 = st_coords[vertex_index[index * 3 + 2]];
		st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
	}

	Vector3f EvalDiffuseColor(const Vector2f& st) const {
		float scale = 5;
		float pattern = (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
		return Lerp(Vector3f(0.815f, 0.235f, 0.031f), Vector3f(0.937f, 0.937f, 0.231f), pattern);
	}

	Bounds3 GetBounds() { return bounding_box; }

	float GetArea() { return area; }

	void Sample(Intersection& pos, float& pdf) {
		bvh->Sample(pos, pdf);
		pos.emit = m->GetEmission();
	}

	bool HasEmit() { return m->HasEmission(); }
};
