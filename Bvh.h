#pragma once
#include "vec3.h"
#include <limits>
#include <utility>
#include <vector>
#include "sphere.h"
#include <algorithm>

struct AABB // Axis-Aligned Bounding Box
{
    vec3 min = { INFINITY };
    vec3 max= { -INFINITY };

    vec3 Center()
    {
        return (max + min) * 0.5f;
    }


};

// BVH Node
struct BVHNode
{
    AABB bounds;
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;

    ~BVHNode() 
    {
        delete left;
        delete right;
    }
   

    int sphereIndex = 0;     // Index of first primitive (used for leaf nodes)
    int sphereCount = 0;     // Number of primitives in this node
};


struct BVH
{
public:
    BVHNode* root = nullptr;
    void expand(Sphere& sphere, AABB& aabb);
    // Helper function to build the BVH
    void build(std::vector<Object*>& objs);

    static bool intersecting(Ray& ray, AABB& aabb);
    // Recursive helper function for BVH construction
    BVHNode* buildRecursive(std::vector<Object*>& objs, BVHNode* parent, int depth);
};


