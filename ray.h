#pragma once
#include "vec3.h"

//------------------------------------------------------------------------------
/**
*/
class Ray
{
public:

    // beginning of ray
    vec3 b;

    // magnitude and direction of ray
    vec3 m;

    Ray()
    {

    }
    Ray(vec3 startpoint, vec3 dir) :
        b(startpoint),
        m(dir)
    {

    }

    ~Ray()
    {

    }
};

inline vec3 PointAt(float t)
{
    Ray ray;
    return { ray.b + ray.m * t };
}