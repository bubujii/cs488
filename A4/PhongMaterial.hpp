// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material
{
public:
    PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess);
    PhongMaterial(const glm::vec3 &kd, const glm::vec3 &ks, double shininess, float index_of_refraction, double transparency);
    virtual ~PhongMaterial();

    glm::vec3 m_kd;
    glm::vec3 m_ks;

    double m_shininess;
    float m_index_of_refraction;
    double m_transparency;
};
