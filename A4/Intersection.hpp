
#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

class Intersection
{
public:
    Intersection(glm::vec3 &point, Material *mat, glm::vec3 &normal)
        : point{point}, mat{mat}, normal{normal} {}
    ~Intersection() {}
    glm::vec3 point;
    Material *mat;
    glm::vec3 normal;
};