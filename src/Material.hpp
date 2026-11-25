#pragma once
# include <cfloat>
# include "Global.hpp"
# include "Vector.hpp"

enum MaterialType {
	DIFFUSE,
};

class Material {
public:
	MaterialType m_type;
	Vector3f m_emission;
	float ior;
	Vector3f k_d, k_s; // k_diffuse, k_specular
	float specular_exponent;
	// Vector3f m_color;
	// Texture tex;

	inline Material(MaterialType t = DIFFUSE, Vector3f e = Vector3f(0.f, 0.f, 0.f));
	inline MaterialType GetType();

	//inline Vector3f GetColor();
	inline Vector3f GetColorAt(double u, double v);

	inline bool HasEmission();
	inline Vector3f GetEmission();

	inline Vector3f Sample(const Vector3f& wi, const Vector3f& N);
	inline float Pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);
	inline Vector3f Eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N);

private:
	Vector3f Reflect(const Vector3f& I, const Vector3f& N) const {
		return I - 2 * DotProduct(I, N) * N;
	}

	Vector3f Refract(const Vector3f& I, const Vector3f& N, const float& ior) const {
		float cosi = clamp(-1, 1, DotProduct(I, N));
		float etai = 1, etat = ior;
		Vector3f n = N;

		if (cosi < 0) { cosi = -cosi; }
		else { 
			std::swap(etai, etat); 
			n = -N;
		}

		float eta = etai / etat;
		float k = 1 - eta * eta * (1 - cosi * cosi);

		return (k < 0) ? 0 : (eta * I + (eta * cosi - sqrtf(k)) * n);
	}

	void Fresnel(const Vector3f& I, const Vector3f& N, const float& ior, float& kr) const {
		float cosi = clamp(-1, 1, DotProduct(I, N));
		float etai = 1, etat = ior;
		if (cosi > 0) { std::swap(etai, etat); }
		// Snell's law
		float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
		// Total internal reflection
		if (sint >= 1) {
			kr = 1;
		}
		else {
			float cost = sqrtf(std::max(0.f, 1 - sint * sint));
			cosi = fabsf(cosi);
			float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
			float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
			kr = (Rs * Rs + Rp * Rp) / 2;
		}
	}

	Vector3f ToWorld(const Vector3f& a, const Vector3f& N) {
		Vector3f B, C;
		if (std::fabs(N.x) > std::fabs(N.y)) {
			float inv_len = 1.f / std::sqrt(N.x * N.x + N.z * N.z);
			C = Vector3f(N.z * inv_len, 0.f, -N.x * inv_len);
		}
		else {
			float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
			C = Vector3f(0.f, N.z * invLen, -N.y * invLen);
		}
		B = CrossProduct(C, N);

		return a.x * B + a.y * C + a.z * N;
	}
};


Material::Material(MaterialType t, Vector3f e) :
	m_type(t), m_emission(e)
{}

MaterialType Material::GetType() { return m_type; }

Vector3f Material::GetColorAt(double u, double v) { return Vector3f(); }

bool Material::HasEmission() {
	if (m_emission.magnitude() > FLT_EPSILON) { return true; }
	else { return false; }
}

Vector3f Material::GetEmission() { return m_emission; }

Vector3f Material::Sample(const Vector3f& wi, const Vector3f& N) {
	switch (m_type) {
	case DIFFUSE:
	{
		// Sample uniformly over the hemisphere around the surface normal.
		float x_1 = get_random_float(), x_2 = get_random_float();
		float z = std::fabs(1.0f - 2.0f * x_1);
		float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
		Vector3f localRay(r * std::cos(phi), r * std::sin(phi), z);
		return ToWorld(localRay, N);
	}
	}
	return Vector3f();
}

float Material::Pdf(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	switch (m_type) {
	case DIFFUSE:
	{
		if (DotProduct(wo, N) > 0.0f)
			return 0.5f / M_PI;
		else
			return 0.0f;
	}
	}
	return 0.0f;
}

Vector3f Material::Eval(const Vector3f& wi, const Vector3f& wo, const Vector3f& N) {
	switch (m_type) {
	case DIFFUSE:
	{
		float cosalpha = DotProduct(N, wo);
		if (cosalpha > 0.0f) {
			Vector3f diffuse = k_d / M_PI;
			return diffuse;
		}
		else
			return Vector3f(0.0f);
	}
	}
	return Vector3f();
}
