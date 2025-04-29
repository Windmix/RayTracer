#include "Bvh.h"
#include <iostream>

void AABB::expand(const AABB& other)
{
    min.x = std::min(min.x, other.min.x);
    min.y = std::min(min.y, other.min.y);
    min.z = std::min(min.z, other.min.z);
    max.x = std::max(max.x, other.max.x);
    max.y = std::max(max.y, other.max.y);
    max.z = std::max(max.z, other.max.z);
}

float AABB::surfaceArea()
{
    vec3 d = max - min;
    return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

bool Primitive::intersecting(Ray& ray, float& t)
{

    // Use the sphere's intersection method
    HitResult hit = sphere->Intersect(ray, t);
    if (hit.object != nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
      // Return true if there was a hit
}

BVHNode* BVH::build(std::vector<Primitive>& primitives)
{
    std::vector<AABB> aabbs;
    std::vector<Primitive*> prims;

    for (auto& primitive : primitives)
    {
        aabbs.push_back(primitive.bounds);
        prims.push_back(&primitive);
    }

    return buildRecursive(aabbs, prims, 0, aabbs.size());
}

BVHNode* BVH::buildRecursive(std::vector<AABB>& aabbs, std::vector<Primitive*>& prims, int start, int end)
{
    if (end - start == 1)
    {
        // Leaf node
        BVHNode* node = new BVHNode(aabbs[start]);
        node->primCount = 1;
        node->firstPrim = start;
        return node;
    }

    // Compute the bounding box for this node
    AABB nodeBounds;
    for (int i = start; i < end; ++i)
    {
        nodeBounds.expand(aabbs[i]);
    }

    // Split the primitives(simplified median split)
    int mid = (start + end) / 2;
    std::nth_element(prims.begin() + start, prims.begin() + mid, prims.begin() + end, [&](Primitive* a, Primitive* b)
        {
            return a->bounds.min.x < b->bounds.min.x;  // Split by x-axis; you could use other strategies for better performance
        });
    BVHNode* node = new BVHNode(nodeBounds);
    node->left = buildRecursive(aabbs, prims, start, mid);
    node->right = buildRecursive(aabbs, prims, mid, end);

    return node;
}
