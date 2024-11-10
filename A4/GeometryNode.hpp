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

    Intersection *intersect(std::pair<glm::vec4, glm::vec4> ray) override;

    void setMaterial(Material *material);

    Material *m_material;
    Primitive *m_primitive;
};
