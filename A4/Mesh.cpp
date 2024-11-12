// Termm--Fall 2024

#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh(const std::string &fname)
	: m_vertices(), m_faces(), bounding_box(glm::vec3(), 1.0)
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs(("Assets/" + fname).c_str());
	glm::vec3 lower_left;
	glm::vec3 upper_right;
	while (ifs >> code)
	{
		if (code == "v")
		{
			ifs >> vx >> vy >> vz;
			m_vertices.push_back(glm::vec3(vx, vy, vz));
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
	glm::vec3 radius_max = upper_right - lower_left;
	double radius = glm::max(glm::max(radius_max.x, radius_max.y), radius_max.z);
	bounding_box = NonhierBox(lower_left, radius);

	std::cout << "Loaded " << m_vertices.size() << " vertices." << std::endl;
}

std::pair<glm::vec3, glm::vec3> *Mesh::intersect(std::pair<glm::vec3, glm::vec3> ray)
{
	auto bounding_intersect = bounding_box.intersect(ray);
	if (!bounding_intersect)
	{
		return nullptr;
	}
#ifdef RENDER_BOUNDING_VOLUMES
	return bounding_intersect;
#endif
	delete bounding_intersect;
	auto epsilon = 0.000001;
	auto total_faces = m_faces.size();
	auto origin = glm::vec3(ray.first);
	auto direction = glm::vec3(ray.second) - origin;
	bool intersected = false;
	auto intersect_point = glm::vec3(0.0);
	auto normal = glm::vec3(0.0);
	auto distance = DBL_MAX;

	// std::cout << "----------------" << std::endl;
	// std::cout << glm::to_string(direction) << std::endl;
	// std::cout << glm::to_string(ray.first) << std::endl;
	// std::cout << glm::to_string(m_vertices[m_faces[0].v1]) << std::endl;
	// std::cout << "----------------" << std::endl;

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
		auto u = (double)glm::dot(ray_to_vertex, ray_cross_e2) * inverse_det;
		if (u < 0 || u > 1)
		{
			continue;
		}

		auto ray_to_vertex_cross_e1 = glm::cross(ray_to_vertex, e1);
		auto v = (double)glm::dot(direction, ray_to_vertex_cross_e1) * inverse_det;
		if (v < 0 || u + v > 1)
		{
			continue;
		}
		auto t_val = (double)glm::dot(e2, ray_to_vertex_cross_e1) * inverse_det;
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
		}
	}
	if (intersected)
	{
		return new std::pair<glm::vec3, glm::vec3>(intersect_point, normal);
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
