// Termm--Fall 2024

#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

#include "Intersection.hpp"

#include <list>
#include <string>
#include <iostream>

enum class NodeType
{
    SceneNode,
    GeometryNode,
    JointNode
};

class SceneNode
{
public:
    SceneNode(const std::string &name);

    SceneNode(const SceneNode &other);

    virtual Intersection *intersect(std::pair<glm::dvec3, glm::dvec3> ray, bool shadow_ray = false);

    virtual ~SceneNode();

    int totalSceneNodes() const;

    const glm::dmat4 &get_transform() const;
    const glm::dmat4 &get_inverse() const;

    void set_transform(const glm::dmat4 &m);

    void add_child(SceneNode *child);

    void remove_child(SceneNode *child);

    //-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::dvec3 &amount);
    void translate(const glm::dvec3 &amount);

    friend std::ostream &operator<<(std::ostream &os, const SceneNode &node);

    // Transformations
    glm::dmat4 trans;
    glm::dmat4 invtrans;

    std::list<SceneNode *> children;

    NodeType m_nodeType;
    std::string m_name;
    unsigned int m_nodeId;

private:
    // The number of SceneNode instances.
    static unsigned int nodeInstanceCount;
};

class NoInterceptException : public std::exception
{
};