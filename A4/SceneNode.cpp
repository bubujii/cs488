// Termm--Fall 2024

#include "SceneNode.hpp"
#include "PhongMaterial.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;

//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string &name)
    : m_name(name),
      m_nodeType(NodeType::SceneNode),
      trans(mat4()),
      invtrans(mat4()),
      m_nodeId(nodeInstanceCount++)
{
}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode &other)
    : m_nodeType(other.m_nodeType),
      m_name(other.m_name),
      trans(other.trans),
      invtrans(other.invtrans)
{
    for (SceneNode *child : other.children)
    {
        this->children.push_front(new SceneNode(*child));
    }
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode()
{
    for (SceneNode *child : children)
    {
        delete child;
    }
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::dmat4 &m)
{
    trans = m;
    invtrans = glm::inverse(m);
}

//---------------------------------------------------------------------------------------
const glm::dmat4 &SceneNode::get_transform() const
{
    return trans;
}

//---------------------------------------------------------------------------------------
const glm::dmat4 &SceneNode::get_inverse() const
{
    return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode *child)
{
    children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode *child)
{
    children.remove(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle)
{
    vec3 rot_axis;

    switch (axis)
    {
    case 'x':
        rot_axis = vec3(1, 0, 0);
        break;
    case 'y':
        rot_axis = vec3(0, 1, 0);
        break;
    case 'z':
        rot_axis = vec3(0, 0, 1);
        break;
    default:
        break;
    }
    dmat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
    set_transform(rot_matrix * trans);
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::dvec3 &amount)
{
    set_transform(glm::scale(amount) * trans);
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::dvec3 &amount)
{
    set_transform(glm::translate(amount) * trans);
}

//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const
{
    return nodeInstanceCount;
}

//---------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream &os, const SceneNode &node)
{

    // os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
    switch (node.m_nodeType)
    {
    case NodeType::SceneNode:
        os << "SceneNode";
        break;
    case NodeType::GeometryNode:
        os << "GeometryNode";
        break;
    case NodeType::JointNode:
        os << "JointNode";
        break;
    }
    os << ":[";

    os << "name:" << node.m_name << ", ";
    os << "id:" << node.m_nodeId;

    os << "]\n";

    for (const SceneNode *child : node.children)
    {
        os << "  " << *child;
    }
    return os;
}

Intersection *SceneNode::intersect(std::pair<glm::dvec3, glm::dvec3> ray, bool shadow_ray)
{
    ray.first = glm::dvec3(invtrans * glm::vec4(ray.first, 1.0));
    ray.second = glm::dvec3(invtrans * glm::vec4(ray.second, 1.0));
    std::vector<Intersection *> intersections;
    for (auto child : children)
    {
        intersections.push_back(child->intersect(ray));
    }
    double closest_distance = DBL_MAX;
    Intersection *intersect = nullptr;
    bool found_intersect = false;
    for (auto intersection : intersections)
    {
        if (!intersection)
            continue;
        double distance = glm::distance(ray.first, intersection->point);
        if (shadow_ray)
        {
            PhongMaterial *mat = (PhongMaterial *)intersection->mat;
            if (mat->m_transparency)
            {
                delete intersection;
                continue;
            }
        }
        if (distance < closest_distance && glm::dot(intersection->point - ray.first, ray.second - ray.first) > 0)
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
