// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material
{
public:
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess);
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, double index_of_refraction, double transparency);
    virtual ~PhongMaterial();

    glm::dvec3 m_kd;
    glm::dvec3 m_ks;

    double m_shininess;
    double m_index_of_refraction;
    double m_transparency;
};
