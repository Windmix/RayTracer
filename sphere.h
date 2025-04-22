#pragma once
#include "object.h"
#include <stdlib.h>
#include <time.h>
#include "mat4.h"
#include "pbr.h"
#include "random.h"
#include "ray.h"
#include "material.h"


// OBJECT

struct HitResult
{
    // hit point
    vec3 p;
    // normal
    vec3 normal;
    // hit object, or nullptr
    Sphere* object = nullptr;
    // intersection distance
    float t = FLT_MAX;
};

// returns a random point on the surface of a unit sphere
inline vec3 random_point_on_unit_sphere()
{
    float x = RandomFloatNTP();
    float y = RandomFloatNTP();
    float z = RandomFloatNTP();
    vec3 v( x, y, z );
    return normalize(v);
}

// a spherical object
class Sphere
{
public:
    //OBJ
    volatile bool isBigObject = false;
    volatile char* name;
    unsigned long long id;
    std::string purpose;

    float radius;
    vec3 center;
    Material const* const material;

    bool hasValue = false;
    HitResult* value = nullptr;
    Sphere(float radius, vec3 center, Material const* const material) : 
        radius(radius),
        center(center),
        material(material)
    {
        static unsigned long long idCounter = 0;
        id = idCounter++;

        // Reserve characters for naming this object something!
        name = new char[256];
        name[0] = 'U';
        name[1] = 'n';
        name[2] = 'n';
        name[3] = 'a';
        name[4] = 'm';
        name[5] = 'e';
        name[6] = 'd';
        name[7] = '\0';

        purpose = std::string("I don't have a purpose at the moment, but hopefully the programmer that overrides me will give me one. :)");
    }
    ~Sphere()
    {
        
    }

};

Ray ScatterRay(Ray ray, vec3 point, vec3 normal) 
{
    return BSDF(this->material, ray, point, normal);
}

Color GetColor()
{
    return material->color;
}

Optional Intersect(Ray ray, float maxDist)
    HitResult hit;
    vec3 oc = ray.b - this->center;
    vec3 dir = ray.m;
    float b = dot(oc, dir);

    // early out if sphere is "behind" ray
    if (b > 0)
        return Optional();

    float a = dot(dir, dir);
    float c = dot(oc, oc) - this->radius * this->radius;
    float discriminant = b * b - a * c;

    if (discriminant > 0)
    {
        constexpr float minDist = 0.001f;
        float div = 1.0f / a;
        float sqrtDisc = sqrt(discriminant);
        float temp = (-b - sqrtDisc) * div;
        float temp2 = (-b + sqrtDisc) * div;

        if (temp < maxDist && temp > minDist)
        {
            vec3 p = PointAt(temp);
            hit.p = p;
            hit.normal = (p - this->center) * (1.0f / this->radius);
            hit.t = temp;
            hit.object = this;
            return Optional(hit);
        }
        if (temp2 < maxDist && temp2 > minDist)
        {
            vec3 p = PointAt(temp2);
            hit.p = p;
            hit.normal = (p - this->center) * (1.0f / this->radius);
            hit.t = temp2;
            hit.object = this;
            return Optional(hit);
        }
    }

    return Optional<HitResult>();
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
        if (value != nullptr)
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