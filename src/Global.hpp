#pragma once

# include <algorithm>
# include <iostream>
# include <cmath>
# include <random>

#undef M_PI
#define M_PI 3.141592653589793f

inline float clamp(const float& lo, const float& hi, const float& value) {
	return std::max(lo, std::min(hi, value));
}

inline float get_random_float() {
	// Each render thread owns its random number generator.
	thread_local std::random_device dev;
	thread_local std::mt19937 rng(dev());
	std::uniform_real_distribution<float> dist(0.f, 1.f);

	return dist(rng);
}

inline void UpdateProgress(float progress)
{
	int barWidth = 70;

	std::cout << "[";

	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";      
		else if (i == pos) std::cout << ">";
		else std::cout << " ";				
	}

	std::cout << "] " << int(progress * 100.0) << " %\r";

	std::cout.flush();
};