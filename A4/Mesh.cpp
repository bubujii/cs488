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
    size_t n1, n2, n3;

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
            if (m_normals.size() > 0)
            {
                char dummy;

                ifs >> s1;
                ifs >> dummy >> dummy >> n1;
                ifs >> s2;
                ifs >> dummy >> dummy >> n2;
                ifs >> s3;
                ifs >> dummy >> dummy >> n3;
                std::cout << s1 << " " << n1 << " " << s2 << " " << n2 << " " << s3 << " " << n3 << std::endl;
                m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1, n1 - 1, n2 - 1, n3 - 1));
                continue;
            }
            ifs >> s1 >> s2 >> s3;
            m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1));
        }
        else if (code == "vn")
        {
            ifs >> vx >> vy >> vz;
            m_normals.push_back(glm::dvec3(vx, vy, vz));
        }
    }
    plane_bottom_left = lower_left;
    plane_bottom_left.y = 0;
    plane_top_right = upper_right;
    plane_top_right.y = 0;

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

    auto center = (upper_right + lower_left) / 2.0;
    bounding_box.translate(center);
    // bounding_box.translate(lower_left);

    std::cout
        << "Loaded " << m_vertices.size() << " vertices." << std::endl;
    std::cout
        << "Loaded " << m_faces.size() << " faces." << std::endl;
    std::cout
        << "Loaded " << m_normals.size() << " normals." << std::endl;
}

PrimitiveHit *Mesh::intersect(std::pair<glm::dvec3, glm::dvec3> ray)
{
    // std::cout << "Intersecting Mesh" << std::endl;
    auto bounding_intersect = bounding_box.intersect(ray);
    if (!bounding_intersect)
    {
        return nullptr;
    }
#ifdef RENDER_BOUNDING_VOLUMES
    PrimitiveHit *bounding_intersect_pair = new PrimitiveHit(bounding_intersect->point, bounding_intersect->normal);
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
            if (m_normals.size() > 0)
            {
                auto n1 = m_normals[m_faces[i].n1];
                auto n2 = m_normals[m_faces[i].n2];
                auto n3 = m_normals[m_faces[i].n3];
                normal = glm::normalize(n1 * (1 - u - v) + n2 * u + n3 * v);
            }
        }
    }
    if (intersected)
    {
        return new PrimitiveHit(intersect_point, normal);
    }
    return nullptr;
}

glm::dvec2 Mesh::uv_map(glm::dvec3 point)
{
    // project onto the xz plane for now and stop complaining
    glm::dvec3 midpoint = (plane_bottom_left + plane_top_right) / 2.0;
    std::cout << "pre-projection" << std::endl;
    std::cout << glm::to_string(point + plane_top_right) << std::endl;
    return (glm::dvec2(point.x, point.z) + glm::dvec2(plane_top_right.x, plane_top_right.z)) * 0.5;
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
