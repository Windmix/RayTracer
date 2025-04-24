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
        // check if this object has a value.
        if (this->hasValue)
        {
            // doublecheck the value
            if (value == nullptr)
            {
                return false;
            }
            else
            {
                // doublecheck the value content.
                if (value->object == nullptr)
                {
                    return false;
                }
                if (value->normal.IsZero())
                {
                    return false;
                }
            }
        }
        else
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
class Object
{
public:
    Object() 
    {
        static std::atomic<unsigned long long> idCounter(0);
        id = idCounter.fetch_add(1, std::memory_order_relaxed);

        // Use std::string for name, eliminates manual memory management
        name = "Unnamed";
        purpose = "I don't have a purpose at the moment, but hopefully the programmer who overrides me will give me one. :)";
    }

    virtual ~Object() = default;

    virtual Optional Intersect(Ray ray, float maxDist) { return {}; };
    virtual Color GetColor() = 0;
    virtual Ray ScatterRay(Ray ray, vec3 point, vec3 normal) 
    {
        return Ray({ 0,0,0 }, {1,1,1});
    };
    std::string GetName() { return name; }

    unsigned long long GetId() { return this->id; }

private:
    volatile bool isBigObject = false;
    std::string name;
    unsigned long long id;
    std::string purpose;
};