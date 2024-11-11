// Termm--Fall 2024

#include "Primitive.hpp"
#include <iostream>
#include <glm/ext.hpp>

Primitive::~Primitive()
{
}

std::pair<glm::vec4, glm::vec4> *Primitive::intersect(std::pair<glm::vec4, glm::vec4> ray)
{
    return nullptr;
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

std::pair<glm::vec4, glm::vec4> *NonhierSphere::intersect(std::pair<glm::vec4, glm::vec4> ray)
{
    auto oc = m_pos - glm::vec3(ray.first);
    glm::vec3 direction = glm::normalize(glm::vec3(ray.second) - glm::vec3(ray.first));
    auto a = glm::dot(direction, direction);
    auto b = -2.0 * glm::dot(direction, oc);
    auto c = glm::dot(oc, oc) - (m_radius * m_radius);
    auto discriminant = (b * b) - (4 * a * c);
    if (discriminant < 0)
    {
        return nullptr;
    }
    auto t1 = (-b - glm::sign(b) * glm::sqrt(discriminant)) / (2 * a);
    auto t2 = c / (a * t1);
    auto intersect_t_val = glm::min(t1, t2);
    // return pair of intersect point and normal
    glm::vec3 intersect_point = glm::vec3(ray.first + glm::vec4(direction * float(intersect_t_val), 1));
    // std::cout << "----------------" << std::endl;
    // std::cout << glm::to_string(direction) << std::endl;
    // std::cout << glm::to_string(ray.first) << std::endl;
    // std::cout << glm::to_string(ray.second) << std::endl;
    // std::cout << intersect_t_val << std::endl;
    // std::cout << glm::to_string(intersect_point) << std::endl;
    // std::cout << glm::to_string(m_pos) << std::endl;
    // std::cout << glm::to_string(glm::normalize(intersect_point - m_pos)) << std::endl;
    // std::cout << "----------------" << std::endl;

    return new std::pair<glm::vec4, glm::vec4>(
        glm::vec4(intersect_point, 1),
        glm::vec4(glm::normalize(intersect_point - m_pos), 0));
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::~NonhierBox()
{
}
