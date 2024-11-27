// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material
{
public:
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess);
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency = 0.0, double reflectivity = 0.0);
    virtual ~PhongMaterial();

    glm::dvec3 m_kd;
    glm::dvec3 m_ks;
    glm::dvec3 m_ke;

    double m_shininess;
    double m_index_of_refraction;
    double m_transparency;
    double m_reflectivity;
    int m_transparency_glossiness;
    int m_reflectivity_glossiness;
};
