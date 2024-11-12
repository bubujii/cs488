
#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

class Intersection
{
public:
    Intersection(glm::dvec3 &point, Material *mat, glm::dvec3 &normal)
        : point{point}, mat{mat}, normal{normal} {}
    ~Intersection() {}
    glm::dvec3 point;
    Material *mat;
    glm::dvec3 normal;
};