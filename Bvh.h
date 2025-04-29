#pragma once
#include "vec3.h"
#include <limits>
#include <utility>
#include <vector>
#include "sphere.h"
#include <algorithm>

struct AABB // Axis-Aligned Bounding Box
{
    vec3 min, max;

    AABB() :
        min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),   // Initialize all components of min
        max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()) // Initialize all components of max
    {
    }

    AABB(const vec3& min, const vec3& max) : min(min), max(max) {}

    void expand(const AABB& other);
   

    float surfaceArea();
};

// BVH Node
struct BVHNode
{
    AABB bounds;
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;

    int firstPrim = 0;     // Index of first primitive (used for leaf nodes)
    int primCount = 0;     // Number of primitives in this node

    BVHNode(const AABB& bounds) : bounds(bounds) {}
    ~BVHNode() 
    {
        delete left;
        delete right;
    }

    bool isLeaf() const { return primCount > 0; }
};

struct Primitive  
{
    AABB bounds; // Bounding box for the sphere
    Sphere* sphere;  // Pointer to the actual sphere object

    // Constructor for a Sphere primitive
    Primitive(Sphere* sphere)
        : sphere(sphere)
    {
        // Set the bounds of the primitive to match the bounding box of the sphere
        bounds.min = sphere->center - vec3(sphere->radius, sphere->radius, sphere->radius);
        bounds.max = sphere->center + vec3(sphere->radius, sphere->radius, sphere->radius);
    }

    bool intersecting(Ray& ray, float& t);
};

struct BVH
{
public:
    BVHNode* root = nullptr;

    // Helper function to build the BVH
    BVHNode* build(std::vector<Primitive>& primitives);
private:

    // Recursive helper function for BVH construction
    BVHNode* buildRecursive(std::vector<AABB>& aabbs, std::vector<Primitive*>& prims, int start, int end);
};


