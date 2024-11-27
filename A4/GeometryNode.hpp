// Termm--Fall 2024

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"

class GeometryNode : public SceneNode
{
public:
    GeometryNode(const std::string &name, Primitive *prim,
                 Material *mat = nullptr);
    ~GeometryNode();

    Intersection *intersect(std::pair<glm::dvec3, glm::dvec3> ray, bool shadow_ray = false) override;

    void setMaterial(Material *material);

    Material *m_material;
    Primitive *m_primitive;
};
