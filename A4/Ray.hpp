#pragma once
#include <glm/glm.hpp>
class Ray
{
public:
    glm::dvec3 origin;
    glm::dvec3 target;
    Ray(glm::dvec3 origin, glm::dvec3 direction) : origin(origin), target(direction) {}
    Ray() : origin(glm::dvec3(0)), target(glm::dvec3(0)) {}
    glm::dvec3 point_at_parameter(double t) const
    {
        return origin + t * (target - origin);
    }
};