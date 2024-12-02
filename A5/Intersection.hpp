
#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

class Intersection
{
public:
    Intersection(glm::dvec3 &point, Material *mat, glm::dvec3 &normal, bool edge_hit = false)
        : point{point}, mat{mat}, normal{normal}, edge_hit{edge_hit} {}
    ~Intersection() {}
    glm::dvec3 point;
    Material *mat;
    glm::dvec3 normal;
    bool edge_hit;
};