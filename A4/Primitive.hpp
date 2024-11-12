// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>
#include <utility>
#include "Intersection.hpp"

class Primitive
{
public:
    virtual ~Primitive();
    virtual std::pair<glm::vec3, glm::vec3> *intersect(std::pair<glm::vec3, glm::vec3> ray);
};

class Sphere : public Primitive
{
public:
    std::pair<glm::vec3, glm::vec3> *intersect(std::pair<glm::vec3, glm::vec3> ray) override;
    virtual ~Sphere();
};

class Cube : public Primitive
{
public:
    virtual ~Cube();
};

class NonhierSphere : public Primitive
{
public:
    NonhierSphere(const glm::vec3 &pos, double radius)
        : m_pos(pos), m_radius(radius)
    {
    }
    std::pair<glm::vec3, glm::vec3> *intersect(std::pair<glm::vec3, glm::vec3> ray) override;
    virtual ~NonhierSphere();

private:
    glm::vec3 m_pos;
    double m_radius;
};

class NonhierBox : public Primitive
{
public:
    NonhierBox(const glm::vec3 &pos, double size)
        : m_pos(pos), m_size(size)
    {
    }
    std::pair<glm::vec3, glm::vec3> *intersect(std::pair<glm::vec3, glm::vec3> ray) override;
    virtual ~NonhierBox();

private:
    glm::vec3 m_pos;
    double m_size;
};
