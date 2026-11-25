# include "Scene.hpp"

void Scene::BuildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::Intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::SampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->HasEmit()){
            emit_area_sum += objects[k]->GetArea();
        }
    }
    // Select an emissive mesh with probability proportional to its area.
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->HasEmit()){
            emit_area_sum += objects[k]->GetArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

Vector3f Scene::CastRay(const Ray &ray, int depth) const
{
    // Misses contribute no light. Emissive surfaces contribute their radiance.
    Intersection p_inter = Intersect(ray);
    if (!p_inter.happened) {
        return Vector3f();
    }
    if (p_inter.m->HasEmission()) {
        return p_inter.m->GetEmission();
    }

    constexpr float shadowTolerance = 0.0001f;
    Vector3f l_dir;
    Vector3f l_indir;

    // Estimate direct lighting by sampling a point on an emissive mesh.
    Intersection x_inter;
    float pdf_light = 0.0f;
    SampleLight(x_inter, pdf_light);

    Vector3f p = p_inter.coords;
    Vector3f x = x_inter.coords;
    Vector3f ws_dir = (x - p).normalize();
    float ws_distance = (x - p).magnitude();
    Vector3f N = p_inter.normal.normalize();
    Vector3f NN = x_inter.normal.normalize();
    Vector3f emit = x_inter.emit;

    // A shadow ray tests visibility up to the sampled point.
    Ray ws_ray(p, ws_dir);
    Intersection ws_ray_inter = Intersect(ws_ray);
    if (ws_ray_inter.distance - ws_distance > -shadowTolerance) {
        l_dir = emit * p_inter.m->Eval(ray.direction, ws_ray.direction, N)
            * DotProduct(ws_ray.direction, N)
            * DotProduct(-ws_ray.direction, NN)
            / std::pow(ws_distance, 2)
            / pdf_light;
    }

    // Russian roulette terminates paths and surviving paths are reweighted.
    if (get_random_float() > russian_roulette) {
        return l_dir;
    }

    l_indir = 0.0;

    Vector3f wi_dir = p_inter.m->Sample(ray.direction, N).normalize();
    Ray wi_ray(p_inter.coords, wi_dir);
    // Skip emissive hits here because direct lighting already samples them.
    Intersection wi_inter = Intersect(wi_ray);
    if (wi_inter.happened && (!wi_inter.m->HasEmission())) {
        l_indir = CastRay(wi_ray, depth + 1) * p_inter.m->Eval(ray.direction, wi_ray.direction, N)
            * DotProduct(wi_ray.direction, N)
            / p_inter.m->Pdf(ray.direction, wi_ray.direction, N)
            / russian_roulette;
    }

    return l_dir + l_indir;
}