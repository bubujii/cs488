// Termm--Fall 2024

#include "PhongMaterial.hpp"
#include <iostream>
#include <glm/ext.hpp>

PhongMaterial::PhongMaterial(
    const glm::vec3 &kd, const glm::vec3 &ks, double shininess)
    : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
    m_index_of_refraction = 0.0;
    m_transparency = 0.0;
}

PhongMaterial::PhongMaterial(
    const glm::vec3 &kd, const glm::vec3 &ks, double shininess, float index_of_refraction, double transparency)
    : m_kd(kd), m_ks(ks), m_shininess(shininess), m_index_of_refraction(index_of_refraction), m_transparency(transparency)
{
    std::cout << "m_kd: " << glm::to_string(m_kd) << "refraction: " << m_index_of_refraction << " transparency: " << m_transparency << std::endl;
}

PhongMaterial::~PhongMaterial()
{
}
