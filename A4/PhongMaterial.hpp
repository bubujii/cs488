// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "Image.hpp"

class PhongMaterial : public Material
{
public:
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess);
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency = 0.0, double reflectivity = 0.0);
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency = 0.0, double reflectivity = 0.0, glm::dvec3 absorption = glm::dvec3(0));
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency = 0.0, double reflectivity = 0.0, glm::dvec3 absorption = glm::dvec3(0), double transparency_glossiness = 0, double reflectivity_glossiness = 0);
    PhongMaterial(const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency = 0.0, double reflectivity = 0.0, glm::dvec3 absorption = glm::dvec3(0), double transparency_glossiness = 0, double reflectivity_glossiness = 0, std::string texture_path = "");
    virtual ~PhongMaterial();

    glm::dvec3 m_kd;
    glm::dvec3 m_ks;
    glm::dvec3 m_ke;

    double m_shininess;
    double m_index_of_refraction;
    double m_transparency;
    double m_reflectivity;
    double m_transparency_glossiness;
    double m_reflectivity_glossiness;
    glm::dvec3 m_absorption;
    Image *m_texture = nullptr;
};
