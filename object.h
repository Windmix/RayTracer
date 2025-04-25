#pragma once
#include "ray.h"
#include "color.h"
#include <float.h>
#include <string>
#include <memory>
#include <atomic>


class Object;

//------------------------------------------------------------------------------
/**
*/
struct HitResult
{
    // hit point
    vec3 p;
    // normal
    vec3 normal;
    // hit object, or nullptr
    Object* object = nullptr;
    // intersection distance
    float t = FLT_MAX;
};

class Optional
{
public:
    Optional() {}
    Optional(HitResult hit) : hasValue(true)
    {
        this->value = new HitResult(hit);
    }
    ~Optional()
    {
        delete value;
        value = nullptr;
    }
    

    bool HasValue()
    {
        if (!this->hasValue || value->normal.IsZero())
    {
        return false;
    }

    return true;
    }
    HitResult Get()
    {
        assert(this->HasValue());
        return *value;
    }

private:
    bool hasValue = false;
    HitResult* value = nullptr;
};

//------------------------------------------------------------------------------
/**
*/
struct Object
{
    volatile bool isBigObject = false;
    unsigned long long id;

    float radius;
    vec3 center;

    Object() 
    {
        static std::atomic<unsigned long long> idCounter(0);
        id = idCounter.fetch_add(1, std::memory_order_relaxed);
    }

    virtual ~Object() = default;

    virtual Optional Intersect(Ray ray, float maxDist) = 0;
    virtual Color GetColor() = 0;
    virtual Ray ScatterRay(Ray ray, vec3 point, vec3 normal) = 0;



};