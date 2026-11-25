#pragma once

#include <vector>

#include "BVH.hpp"

class Scene {
public:
    Scene(int w, int h) : width(w), height(h) {}

    int width;
    int height;
    double fov = 40;

    void Add(Object* object) { objects.push_back(object); }
    void BuildBVH();
    Vector3f CastRay(const Ray& ray, int depth) const;

private:
    Intersection Intersect(const Ray& ray) const;
    void SampleLight(Intersection& pos, float& pdf) const;

    float russian_roulette = 0.8f;
    std::vector<Object*> objects;
    BVHAccel* bvh = nullptr;
};
