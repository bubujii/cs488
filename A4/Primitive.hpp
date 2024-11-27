// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>
#include <utility>
#include "Intersection.hpp"

class PrimitiveHit
{
public:
    PrimitiveHit(glm::dvec3 &point, glm::dvec3 &normal, bool edge_hit = false)
        : point{point}, normal{normal}, edge_hit{edge_hit} {}
    ~PrimitiveHit() {}
    glm::dvec3 point;
    glm::dvec3 normal;
    bool edge_hit;
};

class MeshHit : public PrimitiveHit
{
public:
    MeshHit(glm::dvec3 &point, glm::dvec3 &normal, size_t face_index)
        : PrimitiveHit(point, normal), face_index{face_index} {}
    ~MeshHit() {}
    size_t face_index;
};

class Primitive
{
public:
    virtual ~Primitive();
    virtual PrimitiveHit *intersect(std::pair<glm::dvec3, glm::dvec3> ray);
};

class Sphere : public Primitive
{
public:
    PrimitiveHit *intersect(std::pair<glm::dvec3, glm::dvec3> ray) override;
    virtual ~Sphere();
};

class Cube : public Primitive
{
public:
    PrimitiveHit *intersect(std::pair<glm::dvec3, glm::dvec3> ray) override;
    virtual ~Cube();
};

class NonhierSphere : public Primitive
{
public:
    NonhierSphere(const glm::dvec3 &pos, double radius)
        : m_pos(pos), m_radius(radius)
    {
    }
    PrimitiveHit *intersect(std::pair<glm::dvec3, glm::dvec3> ray) override;
    virtual ~NonhierSphere();

private:
    glm::dvec3 m_pos;
    double m_radius;
};

class NonhierBox : public Primitive
{
public:
    NonhierBox(const glm::dvec3 &pos, double size)
        : m_pos(pos), m_size(size)
    {
    }
    PrimitiveHit *intersect(std::pair<glm::dvec3, glm::dvec3> ray) override;
    virtual ~NonhierBox();

private:
    glm::dvec3 m_pos;
    double m_size;
};

class Cylinder : public Primitive
{
public:
    Cylinder()
    {
    }
    PrimitiveHit *intersect(std::pair<glm::dvec3, glm::dvec3> ray) override;
    virtual ~Cylinder();
};