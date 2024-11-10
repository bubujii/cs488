
#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

class Intersection
{
public:
    Intersection(glm::vec4 &point, Material *mat, glm::vec4 &normal)
        : point{point}, mat{mat}, normal{normal} {}
    ~Intersection() {}
    glm::vec4 point;
    Material *mat;
    glm::vec4 normal;
};