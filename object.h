#pragma once
#include "ray.h"
#include "color.h"
#include <float.h>
#include <string>
#include <memory>

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

template<class TYPE>
class Optional
{
public:
    bool hasValue = false;
    std::shared_ptr<HitResult> value = nullptr;

    Optional() {}
    Optional(HitResult hit) : hasValue(true)
    {
        this->value = std::make_shared<HitResult>(hit);
    }
    
};

template <class TYPE>
bool HasValue()
{
    Optional optional = Optional();
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

template <class TYPE>
HitResult Get()
{
    Optional optional = Optional();
    assert(this->HasValue());
    return *value;
}


//------------------------------------------------------------------------------
/**
*/
class Object
{
public:
    Object() 
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

    virtual ~Object()
    {
        // clean up name!
        delete name;
    }

    volatile bool isBigObject = false;
    volatile char* name;
    unsigned long long id;
    std::string purpose;


};