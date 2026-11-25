#include <chrono>
#include <exception>
#include <iostream>

#include "MeshTriangle.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"

int main() try {
    Scene scene(784, 784);

    Material red(DIFFUSE, Vector3f(0.0f));
    red.k_d = Vector3f(0.63f, 0.065f, 0.05f);
    Material green(DIFFUSE, Vector3f(0.0f));
    green.k_d = Vector3f(0.14f, 0.45f, 0.091f);
    Material white(DIFFUSE, Vector3f(0.0f));
    white.k_d = Vector3f(0.725f, 0.71f, 0.68f);
    Material light(DIFFUSE, (8.0f * Vector3f(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) + 15.6f * Vector3f(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + 18.4f * Vector3f(0.737f + 0.642f, 0.737f + 0.159f, 0.737f)));
    light.k_d = Vector3f(0.65f);

    // The six OBJ meshes form the Cornell box and its ceiling light.
    MeshTriangle floor("models/cornellbox/floor.obj", &white);
    MeshTriangle shortbox("models/cornellbox/shortbox.obj", &white);
    MeshTriangle tallbox("models/cornellbox/tallbox.obj", &white);
    MeshTriangle left("models/cornellbox/left.obj", &red);
    MeshTriangle right("models/cornellbox/right.obj", &green);
    MeshTriangle light_("models/cornellbox/light.obj", &light);

    scene.Add(&floor);
    scene.Add(&shortbox);
    scene.Add(&tallbox);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.BuildBVH();

    // Meshes and their materials stay alive until rendering finishes.
    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
} catch (const std::exception& error) {
    std::cerr << "Render failed: " << error.what() << '\n';
    return 1;
}
