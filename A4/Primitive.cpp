// Termm--Fall 2024

#include "Primitive.hpp"
#include <iostream>
#include <glm/ext.hpp>

Primitive::~Primitive()
{
}

std::pair<glm::dvec3, glm::dvec3> *Primitive::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    return nullptr;
}

std::pair<glm::dvec3, glm::dvec3> *Sphere::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    auto m_pos = glm::dvec3(0.0, 0.0, 0.0);
    auto m_radius = 1.0;
    auto oc = m_pos - glm::dvec3(ray.first);
    glm::dvec3 direction = glm::normalize(glm::dvec3(ray.second) - glm::dvec3(ray.first));
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
    if (t1 < 0 && t2 < 0)
    {
        return nullptr;
    }
    auto intersect_t_val = glm::min(t1, t2);
    if (intersect_t_val < 0)
    {
        intersect_t_val = glm::max(t1, t2);
    }
    glm::dvec3 intersect_point = ray.first + direction * intersect_t_val;

    return new std::pair<glm::dvec3, glm::dvec3>(
        intersect_point,
        glm::normalize(intersect_point - m_pos));
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

std::pair<glm::dvec3, glm::dvec3> *NonhierSphere::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    auto oc = m_pos - ray.first;
    glm::dvec3 direction = ray.second - ray.first;
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
    if (t1 < 0 && t2 < 0)
    {
        return nullptr;
    }
    auto intersect_t_val = glm::min(t1, t2);
    if (intersect_t_val < 0)
    {
        intersect_t_val = glm::max(t1, t2);
    }
    glm::dvec3 intersect_point = ray.first + direction * intersect_t_val;

    return new std::pair<glm::dvec3, glm::dvec3>(
        intersect_point,
        glm::normalize(intersect_point - m_pos));
}

NonhierSphere::~NonhierSphere()
{
}

std::pair<glm::dvec3, glm::dvec3> *NonhierBox::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    auto ray_origin = glm::dvec3(ray.first);
    auto ray_second = glm::dvec3(ray.second);
    auto direction = glm::normalize(ray_second - ray_origin);
    double tmin = (m_pos.x - ray_origin.x) / direction.x;
    double tmax = (m_pos.x + m_size - ray_origin.x) / direction.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    double tymin = (m_pos.y - ray_origin.y) / direction.y;
    double tymax = (m_pos.y + m_size - ray_origin.y) / direction.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
    {
        return nullptr;
    }

    if (tymin > tmin)
    {
        tmin = tymin;
    }

    if (tymax < tmax)
    {
        tmax = tymax;
    }

    double tzmin = (m_pos.z - ray_origin.z) / direction.z;
    double tzmax = (m_pos.z + m_size - ray_origin.z) / direction.z;

    if (tzmin > tzmax)
    {
        std::swap(tzmin, tzmax);
    }

    if ((tmin > tzmax) || (tzmin > tmax))
    {
        return nullptr;
    }

    if (tzmin > tmin)
    {
        tmin = tzmin;
    }

    if (tzmax < tmax)
    {
        tmax = tzmax;
    }
    glm::dvec3 intersect_point = ray_origin + tmin * direction;

    glm::dvec3 shitty_normal = intersect_point - (m_pos + glm::dvec3(m_size / 2.0));
    glm::dvec3 normal = shitty_normal;
    double multiple = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (glm::abs(shitty_normal[i]) > multiple)
        {
            multiple = glm::abs(shitty_normal[i]);
            normal = glm::dvec3(0.0);
            normal[i] = glm::sign(shitty_normal[i]);
        }
    }
    // std::cout << glm::to_string(normal) << std::endl;
    return new std::pair<glm::dvec3, glm::dvec3>(intersect_point, normal);
}

NonhierBox::~NonhierBox()
{
}
