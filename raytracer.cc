#include "raytracer.h"
#include <random>


//------------------------------------------------------------------------------
/**
*/
Raytracer::Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces) :
    frameBuffer(frameBuffer),
    rpp(rpp),
    bounces(bounces),
    width(w),
    height(h)
{
    totalRaytracers = 0;
    //frustum = get_identitymat4();
    //view = get_identitymat4();
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::RaytraceWithBVH(int startPixel, int pixelCount, BVHNode* bvhRoot)
{
    static int leet = 1337;
    std::mt19937 generator (leet++);
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    Ray rays[10];
    int x = 0, y = 0;

    for (int i = startPixel; i < startPixel + pixelCount; ++i)
    {
        Color color;
        for (int j = 0; j < this->rpp; ++j)
        {
            x = i % this->width;
            y = i / this->width;

            float u = ((float(x + dis(generator)) * (1.0f / this->width)) * 2.0f) - 1.0f;
            float v = ((float(y + dis(generator)) * (1.0f / this->height)) * 2.0f) - 1.0f;

            vec3 direction = vec3(u, v, -1.0f);
            direction = transform(direction, this->frustum);

            rays[0] =
            {
                get_position(this->view), direction
            };

            color += this->TraceWithBVH(rays, bvhRoot, 0);
        }

        // divide by number of samples per pixel, to get the average of the distribution
        color.r /= this->rpp;
        color.g /= this->rpp;
        color.b /= this->rpp;

        this->frameBuffer[i] += color;
        totalRaytracers += this->rpp;
    }
}

//------------------------------------------------------------------------------
/**
 * @parameter n - the current bounce level
*/
Color Raytracer::TraceWithBVH(Ray* rays, BVHNode* node, unsigned n)
{

    // If we've reached the bounce limit, return black (no light)
    if (n == this->bounces || !node)
    {
        return { 0, 0, 0 };
    }

    vec3 hitPoint;
    vec3 hitNormal;

    Object* hitObject = nullptr;
    float distance = FLT_MAX;

    // Perform raycast to check if the ray intersects an object
    if (RaycastBVH(rays[n], node, hitPoint, hitNormal, hitObject, distance))
    {
        // Scatter the ray based on the object material
        rays[n+1] = hitObject->ScatterRay(rays[n], hitPoint, hitNormal);

        if (n < bounces)
        {
            // Accumulate color based on scattered ray, with recursion
            return hitObject->GetColor() * this->TraceWithBVH(rays, node, n + 1);
        }

        

    }

    // If no intersection occurs, return the background/skybox color
    return this->Skybox(rays[n].m);
}

//------------------------------------------------------------------------------
/**
*/
bool
Raytracer::RaycastBVH(Ray ray, BVHNode* node, vec3& hitPoint, vec3& hitNormal, Object*& hitObject, float& distance)
{
    bool hitSomething = false;
    float closestT = FLT_MAX;
    HitResult closestHit;

    if (!node || !RayIntersectsAABB(ray, node->bounds))
    {
        return false;
    }

    // First, sort the world objects
   // std::sort(world.begin(), world.end());

    if (node->isLeaf())
    {
        for (int i = node->firstPrim; i < node->firstPrim + node->primCount; ++i)
        {
            Primitive& primitive = allPrimitives[i];
            float t;
            if (primitive.intersecting(ray, t) && t < closestT)
            {
                closestT = t;
                closestHit.p = ray.b + ray.m * t;
                closestHit.normal = normalize((closestHit.p - primitive.sphere->center));
                closestHit.object = primitive.sphere;
                hitSomething = true;
            }
        }
    }
    else
    {
        // Recursively test children
        vec3 childHitPoint, childHitNormal;
        Object* childHitObject = nullptr;
        float childDistance;

        if (RaycastBVH(ray, node->left, childHitPoint, childHitNormal, childHitObject, childDistance) && (childDistance < closestT))
        {
            closestT = childDistance;
            hitPoint = childHitPoint;
            hitNormal = childHitNormal;
            hitObject = childHitObject;
            hitSomething = true;
        }

        if (RaycastBVH(ray, node->right, childHitPoint, childHitNormal, childHitObject, childDistance) && (childDistance < closestT))
        {
            closestT = childDistance;
            hitPoint = childHitPoint;
            hitNormal = childHitNormal;
            hitObject = childHitObject;
            hitSomething = true;
        }
    }
    distance = closestT;
    return hitSomething;
}


//------------------------------------------------------------------------------
/**
*/
void
Raytracer::Clear()
{
    for (auto& color : this->frameBuffer)
    {
        color.r = 0.0f;
        color.g = 0.0f;
        color.b = 0.0f;
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Raytracer::UpdateMatrices()
{
    mat4 inverseView = inverse(this->view); 
    mat4 basis = transpose(inverseView);
    this->frustum = basis;
}

bool Raytracer::RayIntersectsAABB(const Ray& ray, const AABB& box)
{
    float tMin = (box.min.x - ray.b.x) / ray.m.x;   
    float tMax = (box.max.x - ray.b.x) / ray.m.x;
    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (box.min.y - ray.b.y) / ray.m.y;
    float tyMax = (box.max.y - ray.b.y) / ray.m.y;
    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax))
        return false;

    if (tyMin > tMin) tMin = tyMin;
    if (tyMax < tMax) tMax = tyMax;

    float tzMin = (box.min.z - ray.b.z) / ray.m.z;
    float tzMax = (box.max.z - ray.b.z) / ray.m.z;
    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax))
        return false;

    return true;
}

//------------------------------------------------------------------------------
/**
*/
Color
Raytracer::Skybox(vec3 direction)
{
    float t = 0.5*(direction.y + 1.0);
    vec3 vec = vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
    return {(float)vec.x, (float)vec.y, (float)vec.z};
}
void
Raytracer::MultiThreadingRayTraceWithBVH(BVHNode* bvhRoot, int threadCount)
{
    int widthHeight2 = frameBuffer.size() / threadCount;
    std::thread th[64];
    for (int i = 0; i < threadCount; i++)
    {
        std::thread t1(&Raytracer::RaytraceWithBVH, this, i * widthHeight2, widthHeight2, bvhRoot);
        th[i] = std::move(t1);
    }

    for (int i = 0; i < threadCount; i++)
    {
        th[i].join();
    }
}
