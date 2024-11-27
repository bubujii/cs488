// Termm--Fall 2024

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh(const std::string &fname)
    : m_vertices(), m_faces(), bounding_box(GeometryNode("Bounding Box", new Cube()))
{
    std::string code;
    double vx, vy, vz;
    size_t s1, s2, s3;

    std::ifstream ifs((fname).c_str());
    if (!ifs)
    {
        ifs = std::ifstream(("Assets/" + fname).c_str());
    }
    glm::dvec3 lower_left;
    glm::dvec3 upper_right;
    while (ifs >> code)
    {
        if (code == "v")
        {
            ifs >> vx >> vy >> vz;
            m_vertices.push_back(glm::dvec3(vx, vy, vz));
            if (vx < lower_left.x)
            {
                lower_left.x = vx;
            }
            if (vy < lower_left.y)
            {
                lower_left.y = vy;
            }
            if (vz < lower_left.z)
            {
                lower_left.z = vz;
            }
            if (vx > upper_right.x)
            {
                upper_right.x = vx;
            }
            if (vy > upper_right.y)
            {
                upper_right.y = vy;
            }
            if (vz > upper_right.z)
            {
                upper_right.z = vz;
            }
        }

        else if (code == "f")
        {
            ifs >> s1 >> s2 >> s3;
            m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1));
        }
    }
    glm::dvec3 corner_distance = upper_right - lower_left;
    if (corner_distance.x == 0)
    {
        corner_distance.x = 0.01;
        lower_left.x -= 0.01;
    }
    if (corner_distance.y == 0)
    {
        corner_distance.y = 0.01;
        lower_left.y -= 0.01;
    }
    if (corner_distance.z == 0)
    {
        corner_distance.z = 0.01;
        lower_left.z -= 0.01;
    }
    bounding_box.scale(corner_distance);
    // bounding_box.translate(lower_left);

    std::cout
        << "Loaded " << m_vertices.size() << " vertices." << std::endl;
}

PrimitiveHit *Mesh::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    auto bounding_intersect = bounding_box.intersect(ray);
    if (!bounding_intersect)
    {
        return nullptr;
    }
#ifdef RENDER_BOUNDING_VOLUMES
    std::pair<glm::dvec3, glm::dvec3> *bounding_intersect_pair = new std::pair<glm::dvec3, glm::dvec3>(bounding_intersect->point, bounding_intersect->normal);
    delete bounding_intersect;
    return bounding_intersect_pair;
#endif
    delete bounding_intersect;
    auto epsilon = 0.000001;
    auto total_faces = m_faces.size();
    auto origin = ray.first;
    auto direction = glm::normalize(ray.second - origin);
    bool intersected = false;
    auto intersect_point = glm::dvec3(0.0);
    auto normal = glm::dvec3(0.0);
    auto distance = DBL_MAX;

    glm::dvec3 v1_hit = glm::dvec3(0.0);
    glm::dvec3 v2_hit = glm::dvec3(0.0);
    glm::dvec3 v3_hit = glm::dvec3(0.0);

    for (size_t i = 0; i < total_faces; i++)
    {
        auto v1 = m_vertices[m_faces[i].v1];
        auto v2 = m_vertices[m_faces[i].v2];
        auto v3 = m_vertices[m_faces[i].v3];
        auto e1 = v2 - v1;
        auto e2 = v3 - v1;
        auto ray_cross_e2 = glm::cross(direction, e2);
        auto det = glm::dot(e1, ray_cross_e2);
        if (det < epsilon && det > -epsilon)
        {
            continue;
        }
        double inverse_det = 1.0 / det;
        auto ray_to_vertex = origin - v1;
        auto u = glm::dot(ray_to_vertex, ray_cross_e2) * inverse_det;
        if (u < 0 || u > 1)
        {
            continue;
        }

        auto ray_to_vertex_cross_e1 = glm::cross(ray_to_vertex, e1);
        auto v = glm::dot(direction, ray_to_vertex_cross_e1) * inverse_det;
        if (v < 0 || u + v > 1)
        {
            continue;
        }
        auto t_val = glm::dot(e2, ray_to_vertex_cross_e1) * inverse_det;
        if (t_val < epsilon)
        {
            continue;
        }
        auto intersect = origin + t_val * direction;
        auto dist = glm::distance(origin, intersect);
        if (dist < distance)
        {
            intersected = true;
            distance = dist;
            intersect_point = intersect;
            normal = glm::normalize(glm::cross(e1, e2));
            v1_hit = v1;
            v2_hit = v2;
            v3_hit = v3;
        }
    }
    if (intersected)
    {
        // auto ray_1 = intersect_point - v1_hit;
        // auto ray_2 = intersect_point - v2_hit;
        // double distance_e1 = glm::length(intersect_point - glm::dot(ray_1, v2_hit - v1_hit) * (v2_hit - v1_hit));
        // double distance_e2 = glm::length(intersect_point - glm::dot(ray_1, v3_hit - v1_hit) * (v3_hit - v1_hit));
        // double distance_e3 = glm::length(intersect_point - glm::dot(ray_2, v3_hit - v2_hit) * (v3_hit - v2_hit));
        // double max_distance = 1;
        // if (distance_e1 < max_distance || distance_e2 < max_distance || distance_e3 < max_distance)
        // {
        //     return new PrimitiveHit(intersect_point, normal, true);
        // }
        return new PrimitiveHit(intersect_point, normal);
    }
    return nullptr;
}

std::ostream &operator<<(std::ostream &out, const Mesh &mesh)
{
    out << "mesh {";
    /*

    for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
      const MeshVertex& v = mesh.m_verts[idx];
      out << glm::to_string( v.m_position );
      if( mesh.m_have_norm ) {
        out << " / " << glm::to_string( v.m_normal );
      }
      if( mesh.m_have_uv ) {
        out << " / " << glm::to_string( v.m_uv );
      }
    }

  */
    out << "}";
    return out;
}
