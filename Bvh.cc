#include "Bvh.h"
#include <iostream>
#include <algorithm>

void BVH::expand(Sphere& sphere, AABB& aabb)
{
    aabb.min.x = std::min(aabb.min.x, sphere.center.x - sphere.radius);
    aabb.min.y = std::min(aabb.min.y, sphere.center.y - sphere.radius);
    aabb.min.z = std::min(aabb.min.z, sphere.center.z - sphere.radius);
    aabb.max.x = std::max(aabb.max.x, sphere.center.x + sphere.radius);
    aabb.max.y = std::max(aabb.max.y, sphere.center.y + sphere.radius);
    aabb.max.z = std::max(aabb.max.z, sphere.center.z + sphere.radius);
}


bool BVH::intersecting(Ray& ray, AABB& aabb)
{
    // r.dir is unit direction vector of ray
    float dirfracX = 1.0f / ray.m.x;
    float dirfracY = 1.0f / ray.m.y;
    float dirfracZ = 1.0f / ray.m.z;
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner

    // r.org is origin of ray
    float t1 = (aabb.min.x - ray.b.x) * dirfracX;
    float t2 = (aabb.max.x - ray.b.x) * dirfracX;
    float t3 = (aabb.min.y - ray.b.y) * dirfracY;
    float t4 = (aabb.max.y - ray.b.y) * dirfracY;
    float t5 = (aabb.min.z - ray.b.z) * dirfracZ;
    float t6 = (aabb.max.z - ray.b.z) * dirfracZ;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    return tmax >= tmin && tmax > 0;
}

void BVH::build(std::vector<Object*>& objs)
{
    root = new BVHNode();
    for (auto obj : objs)
    {
        expand(*dynamic_cast<Sphere*>(obj), root->bounds);
    }
    root->sphereCount = objs.size();


    // Begin recursive build
     buildRecursive(objs, root, 0);
}

BVHNode* BVH::buildRecursive(std::vector<Object*>& objs, BVHNode* parent, int depth = 0)
{
    const int MaxDepth = 32;

    if (depth >= MaxDepth || parent->sphereCount <= 2)
    {
        return nullptr;
    }

    parent->left = new BVHNode();
    parent->right = new BVHNode();
    
    parent->left->sphereIndex = parent->sphereIndex;
    parent->right->sphereIndex = parent->sphereIndex;

    vec3 diagonal = parent->bounds.max - parent->bounds.min;
    int splitAxis = (diagonal.x >= diagonal.y && diagonal.x >= diagonal.z) ? 0  // X is longest
        : (diagonal.y >= diagonal.z) ? 1                                        // Y is longest
        : 2;                                                                    // Z is longest


    for (int i = parent->sphereIndex; i < parent->sphereIndex + parent->sphereCount; i++)
    {
        bool isSideLeft = ((Sphere*)objs[i])->center.y < parent->bounds.Center()[splitAxis];

        BVHNode* child = isSideLeft ? parent->left : parent->right;
        expand(*(Sphere*)objs[i], child->bounds);
        child->sphereCount++;

        if (isSideLeft)
        {
            int swap = child->sphereIndex + child->sphereCount - 1; 
            std::swap(objs[i], objs[swap]);
            parent->right->sphereIndex++;
        }
        
    }
    buildRecursive(objs, parent->left, depth + 1);
    buildRecursive(objs, parent->right, depth + 1);
}

