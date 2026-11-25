# include <algorithm>
# include <atomic>
# include <chrono>
# include <thread>
# include <fstream>
# include <stdexcept>
# include "Scene.hpp"
# include "Renderer.hpp"

inline float Deg2Rad(const float& deg) { return deg * M_PI / 180.f; }

void Renderer::Render(const Scene& scene) {
	std::vector<Vector3f> framebuffer(scene.width * scene.height);

	float scale = tan(Deg2Rad(scene.fov * 0.5));
	float image_aspect_ratio = scene.width / (float)scene.height;

	Vector3f eye_pos(278, 273, -800);

	int spp = 128; // samples per pixel

	std::cout << "SPP: " << spp << "\n";

	const int total_pixels = scene.width * scene.height;
	std::atomic<int> completed_pixels(0);

	const int n_threads = std::max(1u, std::thread::hardware_concurrency());
	std::vector<std::thread> threads;

	auto update_progress = [&]() {
		while (completed_pixels < total_pixels) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			float progress = static_cast<float>(completed_pixels) / total_pixels;
			UpdateProgress(progress);
		}
	};

	std::thread progress_thread(update_progress);

	// Each worker writes to its own contiguous range of pixels.
	auto worker = [&](int start, int end) {
		for (int pixel_idx = start; pixel_idx < end; ++pixel_idx) {
			int i = pixel_idx % scene.width;
			int j = pixel_idx / scene.width;

			// Project the pixel center onto the camera plane.
			float x = (2 * (i + 0.5f) / (float)scene.width - 1) * image_aspect_ratio * scale;
			float y = (1 - 2 * (j + 0.5f) / (float)scene.height) * scale;

			Vector3f dir = Normalize(Vector3f(-x, y, 1));
			Vector3f color(0.f);

			for (int k = 0; k < spp; ++k) {
				color += scene.CastRay(Ray(eye_pos, dir), 0);
			}
			color = color / spp;

			framebuffer[pixel_idx] = color;

			completed_pixels++;
		}
	};

	int chunk_size = total_pixels / n_threads;
	for (int t = 0; t < n_threads; t++) {
		int start = t * chunk_size;
		int end = (t == n_threads - 1) ? total_pixels : (start + chunk_size);
		threads.emplace_back(worker, start, end);
	}

	for (auto& thread : threads) {
		thread.join();
	}

	progress_thread.join();
	UpdateProgress(1.f);

	// Apply the display curve before writing 8-bit RGB values.
	std::ofstream output("binary.ppm", std::ios::binary);
	if (!output) {
		throw std::runtime_error("Could not open binary.ppm for writing.");
	}
	output << "P6\n" << scene.width << ' ' << scene.height << "\n255\n";
	for (const auto& pixel : framebuffer) {
		const unsigned char color[] = {
			static_cast<unsigned char>(255 * std::pow(clamp(0, 1, pixel.x), 0.6f)),
			static_cast<unsigned char>(255 * std::pow(clamp(0, 1, pixel.y), 0.6f)),
			static_cast<unsigned char>(255 * std::pow(clamp(0, 1, pixel.z), 0.6f)),
		};
		output.write(reinterpret_cast<const char*>(color), sizeof(color));
	}
	output.close();
	if (!output) {
		throw std::runtime_error("Could not finish writing binary.ppm.");
	}
}
