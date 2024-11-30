// Termm--Fall 2024

#include "PhongMaterial.hpp"
#include <iostream>
#include <glm/ext.hpp>

PhongMaterial::PhongMaterial(
    const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess)
    : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
    m_index_of_refraction = 0.0;
    m_transparency = 0.0;
}

PhongMaterial::PhongMaterial(
    const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency, double reflectivity)
    : m_kd(kd), m_ks(ks), m_shininess(shininess), m_ke(ke), m_index_of_refraction(index_of_refraction), m_transparency(transparency), m_reflectivity(reflectivity), m_absorption(glm::dvec3(0))
{
    std::cout << "m_kd: " << glm::to_string(m_kd) << "refraction: " << m_index_of_refraction << " transparency: " << m_transparency << " reflectivity: " << m_reflectivity << std::endl;
}

PhongMaterial::PhongMaterial(
    const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency, double reflectivity, glm::dvec3 absorption)
    : m_kd(kd), m_ks(ks), m_shininess(shininess), m_ke(ke), m_index_of_refraction(index_of_refraction), m_transparency(transparency), m_reflectivity(reflectivity), m_absorption(absorption)
{
}

PhongMaterial::PhongMaterial(
    const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency, double reflectivity, glm::dvec3 absorption, double transparency_glossiness, double reflectivity_glossiness)
    : m_kd(kd),
      m_ks(ks),
      m_shininess(shininess),
      m_ke(ke),
      m_index_of_refraction(index_of_refraction),
      m_transparency(transparency),
      m_reflectivity(reflectivity),
      m_absorption(absorption),
      m_transparency_glossiness(transparency_glossiness),
      m_reflectivity_glossiness(reflectivity_glossiness)
{
}

PhongMaterial::PhongMaterial(
    const glm::dvec3 &kd, const glm::dvec3 &ks, double shininess, const glm::dvec3 &ke, double index_of_refraction, double transparency, double reflectivity, glm::dvec3 absorption, double transparency_glossiness, double reflectivity_glossiness, std::string texture_path)
    : m_kd(kd),
      m_ks(ks),
      m_shininess(shininess),
      m_ke(ke),
      m_index_of_refraction(index_of_refraction),
      m_transparency(transparency),
      m_reflectivity(reflectivity),
      m_absorption(absorption),
      m_transparency_glossiness(transparency_glossiness),
      m_reflectivity_glossiness(reflectivity_glossiness)
{
    if (texture_path != "")
    {
        m_texture = new Image(texture_path);
    }
    else
    {
        m_texture = nullptr;
    }
}

PhongMaterial::~PhongMaterial()
{
    if (m_texture)
    {
        delete m_texture;
    }
}
