// Termm--Fall 2024

#pragma once

#include <vector>
#include <iosfwd>
#include "GeometryNode.hpp"
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "Image.hpp"

// Use this #define to selectively compile your code to render the
// bounding boxes around your mesh objects. Uncomment this option
// to turn it on.
// #define RENDER_BOUNDING_VOLUMES

struct Triangle
{
    size_t v1;
    size_t v2;
    size_t v3;

    size_t n1;
    size_t n2;
    size_t n3;

    Triangle(size_t pv1, size_t pv2, size_t pv3)
        : v1(pv1), v2(pv2), v3(pv3)
    {
        n1 = 0;
        n2 = 0;
        n3 = 0;
    }

    Triangle(size_t pv1, size_t pv2, size_t pv3, size_t pn1, size_t pn2, size_t pn3)
        : v1(pv1), v2(pv2), v3(pv3), n1(pn1), n2(pn2), n3(pn3)
    {
    }
};

// A polygonal mesh.
class Mesh : public Primitive
{
public:
    Mesh(const std::string &fname);
    Mesh();
    PrimitiveHit *intersect(std::pair<glm::dvec3, glm::dvec3> ray) override;
    glm::dvec2 uv_map(glm::dvec3 point) override;

protected:
    std::vector<glm::dvec3> m_vertices;
    std::vector<glm::dvec3> m_normals;
    std::vector<Triangle> m_faces;
    GeometryNode bounding_box;
    glm::dvec3 plane_bottom_left;
    glm::dvec3 plane_top_right;

    friend std::ostream &operator<<(std::ostream &out, const Mesh &mesh);
};
