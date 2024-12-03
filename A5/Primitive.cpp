// Termm--Fall 2024

#include "Primitive.hpp"
#include <iostream>
#include <glm/ext.hpp>

Primitive::~Primitive()
{
}

PrimitiveHit *Primitive::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    return nullptr;
}

glm::dvec2 Primitive::uv_map(glm::dvec3 point)
{
    glm::dvec2 uv = glm::dvec2(point.x, point.z) + glm::dvec2(0.5);
    uv.x = glm::clamp(uv.x, 0.0, 1.0);
    uv.y = glm::clamp(uv.y, 0.0, 1.0);
    return uv;
}

PrimitiveHit *intersect_sphere(glm::dvec3 m_pos, double m_radius, std::pair<glm::dvec3, glm::dvec3> ray)
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

    glm::dvec3 normal = glm::normalize(intersect_point - m_pos);
    return new PrimitiveHit(
        intersect_point,
        normal);
}

PrimitiveHit *Sphere::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    return intersect_sphere(glm::dvec3(0.0), 1.0, ray);
}

Sphere::~Sphere()
{
}

Cube::~Cube()
{
}

PrimitiveHit *NonhierSphere::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    return intersect_sphere(m_pos, m_radius, ray);
}

NonhierSphere::~NonhierSphere()
{
}

PrimitiveHit *intersect_cube(glm::dvec3 m_pos, double m_size, std::pair<glm::dvec3, glm::dvec3> ray)
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

    if (tmin < 0)
    {
        tmin = tmax;
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
    return new PrimitiveHit(intersect_point, normal, false);
}

PrimitiveHit *NonhierBox::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{

    return intersect_cube(m_pos, m_size, ray);
}

NonhierBox::~NonhierBox()
{
}

PrimitiveHit *Cube::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    return intersect_cube(glm::dvec3(-0.5), 1.0, ray);
}

PrimitiveHit *cylinder_wall_hit(std::pair<glm::dvec3, glm::dvec3> ray, double height, double radius, bool origin_is_bottom = true)
{
    glm::dvec3 axis = glm::dvec3(0.0, 1.0, 0.0);

    glm::dvec3 d = ray.second - ray.first - glm::dot(ray.second - ray.first, axis) * axis;
    glm::dvec3 oc = ray.first - glm::dot(ray.first, axis) * axis;

    double a = glm::dot(d, d);
    double b = 2 * glm::dot(d, oc);
    double c = glm::dot(oc, oc) - radius * radius;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
    {
        return nullptr;
    }

    double t1 = (-b - glm::sqrt(discriminant)) / (2 * a);
    double t2 = (-b + glm::sqrt(discriminant)) / (2 * a);

    if (t1 < 0 && t2 < 0)
    {
        return nullptr;
    }

    double intersect_t_val = glm::min(t1, t2);
    if (intersect_t_val < 0)
    {
        intersect_t_val = glm::max(t1, t2);
    }
    glm::dvec3 intersect_point = ray.first + (ray.second - ray.first) * intersect_t_val;
    bool hit_cylinder = false;
    double lower_bound = -height / 2;
    if (origin_is_bottom)
    {
        lower_bound = 0;
    }

    if (intersect_point.y < lower_bound || intersect_point.y > lower_bound + height)
    {
        return nullptr;
    }

    glm::dvec3 normal = glm::normalize(glm::dvec3(intersect_point.x, 0.0, intersect_point.z));
    return new PrimitiveHit(intersect_point, normal);
}

PrimitiveHit *cylinder_cap_hit(std::pair<glm::dvec3, glm::dvec3> ray, double height, double radius, bool origin_is_bottom = true)
{
    glm::dvec3 axis = glm::dvec3(0.0, height, 0.0);

    double lower_bound = -height / 2;
    if (origin_is_bottom)
    {
        lower_bound = 0;
    }
    double t1 = (lower_bound - ray.first.y) / (ray.second.y - ray.first.y);
    double t2 = (lower_bound + height - ray.first.y) / (ray.second.y - ray.first.y);

    if (t1 > t2)
    {
        std::swap(t1, t2);
    }

    if (t1 < 0)
    {
        t1 = t2;
    }

    glm::dvec3 intersect_point = ray.first + (ray.second - ray.first) * t1;

    if (intersect_point.x * intersect_point.x + intersect_point.z * intersect_point.z > radius * radius)
    {
        return nullptr;
    }

    glm::dvec3 normal = glm::normalize(glm::dvec3(0.0, glm::sign(lower_bound + height / 2 - intersect_point.y), 0.0));
    return new PrimitiveHit(intersect_point, normal);
}

PrimitiveHit *Cylinder::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    PrimitiveHit *wall_hit = cylinder_wall_hit(ray, m_height, m_radius, m_origin_is_bottom);
    PrimitiveHit *cap_hit = cylinder_cap_hit(ray, m_height, m_radius, m_origin_is_bottom);

    if (wall_hit == nullptr && cap_hit == nullptr)
    {
        return nullptr;
    }

    if (wall_hit == nullptr)
    {
        return cap_hit;
    }

    if (cap_hit == nullptr)
    {
        return wall_hit;
    }

    double wall_t = glm::distance(ray.first, wall_hit->point);
    double cap_t = glm::distance(ray.first, cap_hit->point);

    if (wall_t < cap_t)
    {
        delete cap_hit;
        return wall_hit;
    }
    else
    {
        delete wall_hit;
        return cap_hit;
    }
}

Cylinder::~Cylinder()
{
}
