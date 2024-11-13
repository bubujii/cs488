// Termm--Fall 2024

#pragma once

#include <vector>
#include <iosfwd>
#include "GeometryNode.hpp"
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
// #define RENDER_BOUNDING_VOLUMES

struct Triangle
{
    size_t v1;
    size_t v2;
    size_t v3;

    Triangle(size_t pv1, size_t pv2, size_t pv3)
        : v1(pv1), v2(pv2), v3(pv3)
    {
    }
};

// A polygonal mesh.
class Mesh : public Primitive
{
public:
    Mesh(const std::string &fname);
    std::pair<glm::dvec3, glm::dvec3> *intersect(std::pair<glm::dvec3, glm::dvec3> ray) override;

private:
    std::vector<glm::dvec3> m_vertices;
    std::vector<Triangle> m_faces;
    GeometryNode bounding_box;

    friend std::ostream &operator<<(std::ostream &out, const Mesh &mesh);
};
