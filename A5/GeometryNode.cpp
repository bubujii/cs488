// Termm--Fall 2024

#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include <glm/ext.hpp>

Material *getTextureMaterial(Material *mat, glm::dvec2 uv, glm::dvec2 tiling = glm::dvec2(1.0, 1.0))
{
    if (!mat)
    {
        return nullptr;
    }
    PhongMaterial *phongMat = (PhongMaterial *)mat;
    Image *texture = phongMat->m_texture;
    if (!texture)
    {
        return phongMat;
    }
    glm::dvec3 color;
    uv.x = std::fmod(uv.x, 1.0 / tiling.x) * tiling.x;
    uv.y = std::fmod(uv.y, 1.0 / tiling.y) * tiling.y;
    uv.x = glm::floor(uv.x * texture->width());
    uv.y = glm::floor(uv.y * texture->height());
    uv.x = glm::clamp(uv.x, 0.0, (double)(texture->width() - 1));
    uv.y = glm::clamp(uv.y, 0.0, (double)(texture->height() - 1));
    color.r = (*texture)(uv.x, uv.y, 0);
    color.g = (*texture)(uv.x, uv.y, 1);
    color.b = (*texture)(uv.x, uv.y, 2);
    phongMat->m_kd = color;
    return phongMat;
}

glm::dvec3 getNormal(Material *mat, glm::dvec2 uv, glm::dvec3 cur_normal, glm::dvec2 tiling = glm::dvec2(1.0, 1.0))
{
    if (!mat)
    {
        return cur_normal;
    }
    PhongMaterial *phongMat = (PhongMaterial *)mat;
    Image *normal_map = phongMat->m_normal_map;
    if (!normal_map)
    {
        return cur_normal;
    }
    glm::dvec3 normal;
    uv.x = std::fmod(uv.x, 1.0 / tiling.x) * tiling.x;
    uv.y = std::fmod(uv.y, 1.0 / tiling.y) * tiling.y;
    uv.x = glm::clamp(uv.x, 0.0, 1.0);
    uv.y = glm::clamp(uv.y, 0.0, 1.0);
    uv.x = glm::floor(uv.x * normal_map->width());
    uv.y = glm::floor(uv.y * normal_map->height());
    uv.x = glm::clamp(uv.x, 0.0, (double)(normal_map->width() - 1));
    uv.y = glm::clamp(uv.y, 0.0, (double)(normal_map->height() - 1));
    normal.r = glm::round((*normal_map)(uv.x, uv.y, 0) * 100) / 100;
    normal.g = glm::round((*normal_map)(uv.x, uv.y, 2) * 100) / 100;
    normal.b = glm::round((*normal_map)(uv.x, uv.y, 1) * 100) / 100;

    normal = 2.0 * normal - glm::dvec3(1.0);
    return glm::normalize(normal);
}

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
    const std::string &name, Primitive *prim, Material *mat)
    : SceneNode(name), m_material(mat), m_primitive(prim)
{
    m_nodeType = NodeType::GeometryNode;
    m_texture_tiling = glm::dvec2(1.0, 1.0);
}

GeometryNode::~GeometryNode()
{
    if (m_material)
    {
        delete m_material;
    }
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
        glm::dvec2 uv = m_primitive->uv_map(intersect_point->point);
        glm::dvec3 normal = getNormal(m_material, uv, intersect_point->normal, m_texture_tiling);
        intersections.push_back(new Intersection(
            intersect_point->point,
            getTextureMaterial(m_material, uv, m_texture_tiling),
            normal,
            intersect_point->edge_hit));

        delete intersect_point;
    }
    for (auto child : children)
    {
        intersections.push_back(child->intersect(std::make_pair(ray.first, ray.second)));
    }
    double closest_distance = DBL_MAX;
    Intersection *intersect = nullptr;
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
    for (auto child : children)
    {
        GeometryNode *gChild = dynamic_cast<GeometryNode *>(child);
        if (gChild)
        {
            if (gChild->m_material)
            {
                continue;
            }
            gChild->setMaterial(mat);
        }
    }
}

void GeometryNode::setTextureTiling(glm::dvec2 tiling)
{
    m_texture_tiling = tiling;
}
