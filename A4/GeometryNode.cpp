// Termm--Fall 2024

#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include <glm/ext.hpp>

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
    const std::string &name, Primitive *prim, Material *mat)
    : SceneNode(name), m_material(mat), m_primitive(prim)
{
    m_nodeType = NodeType::GeometryNode;
}

GeometryNode::~GeometryNode()
{
    if (m_material)
    {
        delete m_material;
    };
    if (m_primitive)
    {
        delete m_primitive;
    }
}

Intersection *GeometryNode::intersect(std::pair<glm::dvec3, glm::dvec3> ray, bool shadow_ray)
{
    ray.first = glm::dvec3(invtrans * glm::vec4(ray.first, 1.0));
    ray.second = glm::dvec3(invtrans * glm::vec4(ray.second, 1.0));
    std::vector<Intersection *> intersections;
    auto intersect_point = m_primitive->intersect(ray);
    if (intersect_point)
    {
        intersections.push_back(new Intersection(intersect_point->point, m_material, intersect_point->normal, intersect_point->edge_hit));
        delete intersect_point;
    }
    for (auto child : children)
    {
        intersections.push_back(child->intersect(std::make_pair(ray.first, ray.second)));
    }
    double closest_distance = DBL_MAX;
    Intersection *intersect = nullptr;
    bool found_intersect = false;
    for (auto intersection : intersections)
    {
        if (!intersection)
            continue;
        if (shadow_ray)
        {
            PhongMaterial *mat = (PhongMaterial *)intersection->mat;
            if (mat->m_transparency)
            {
                delete intersection;
                continue;
            }
        }
        double distance = glm::distance(ray.first, intersection->point);

        if (distance < closest_distance)
        {
            delete intersect;
            intersect = intersection;
            closest_distance = distance;
        }
        else
        {
            delete intersection;
        }
    }
    if (intersect)
    {
        intersect->point = glm::dvec3(trans * glm::vec4(intersect->point, 1.0));
        intersect->normal = glm::mat3(glm::transpose(invtrans)) * intersect->normal;
    }
    return intersect;
}

void GeometryNode::setMaterial(Material *mat)
{
    // Obviously, there's a potential memory leak here.  A good solution
    // would be to use some kind of reference counting, as in the
    // C++ shared_ptr.  But I'm going to punt on that problem here.
    // Why?  Two reasons:
    // (a) In practice we expect the scene to be constructed exactly
    //     once.  There's no reason to believe that materials will be
    //     repeatedly overwritten in a GeometryNode.
    // (b) A ray tracer is a program in which you compute once, and
    //     throw away all your data.  A memory leak won't build up and
    //     crash the program.

    m_material = mat;
}
