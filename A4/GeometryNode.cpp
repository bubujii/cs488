// Termm--Fall 2024

#include "GeometryNode.hpp"
#include <glm/ext.hpp>

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
    const std::string &name, Primitive *prim, Material *mat)
    : SceneNode(name), m_material(mat), m_primitive(prim)
{
    m_nodeType = NodeType::GeometryNode;
}

Intersection *GeometryNode::intersect(std::pair<glm::vec4, glm::vec4> ray)
{
    ray.first = invtrans * ray.first;
    ray.second = invtrans * ray.second;
    std::vector<Intersection *> intersections;
    auto intersect_point = m_primitive->intersect(ray);
    if (intersect_point)
    {
        intersections.push_back(new Intersection(intersect_point->first, m_material, intersect_point->second));
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
        if (glm::dot(ray.second - ray.first, intersection->point - ray.first) < 0)
            continue;
        // if (!intersection || glm::dot(intersection->point - ray.first, ray.second - ray.first) < 0)
        // {
        //     delete intersection;
        //     continue;
        // }
        double distance = glm::distance(ray.first, intersection->point);
        if (distance < closest_distance)
        {
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
        intersect->point = trans * intersect->point;
        intersect->normal = glm::vec4(glm::mat3(glm::transpose(invtrans)) * glm::vec3(intersect->normal), 0.0);
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
