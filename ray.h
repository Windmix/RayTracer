#pragma once
#include "vec3.h"

//------------------------------------------------------------------------------
/**
*/
struct Ray
{
public:
    // beginning of ray
    vec3 b;
    // magnitude and direction of ray
    vec3 m;
   
};

inline vec3 PointAt(float t, Ray& ray)
{
    
    return { ray.b + ray.m * t };
}