#include "FractalMountain.hpp"
#include "Mesh.hpp"
#include <iostream>
#include <random>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <limits>

FractalMountain::FractalMountain(
    int grid_size, double roughness)
    : Mesh(), grid_size(grid_size), roughness(roughness)
{
    height_map = std::vector<std::vector<double>>(grid_size, std::vector<double>(grid_size, 0.0));
    double offset = -grid_size;
    peaks.push_back(glm::dvec3(0, 11, 0));
    peaks.push_back(glm::dvec3(-3, 10, -4));
    peaks.push_back(glm::dvec3(2, 6, -4));
    peaks.push_back(glm::dvec3(-4, 6, -0.3));
    peaks.push_back(glm::dvec3(-10, 6, -0.3));
    generate();
}

void FractalMountain::generate()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-1.0, 1.0);

    height_map[0][0] = dist(gen);
    height_map[0][grid_size - 1] = dist(gen);
    height_map[grid_size - 1][0] = dist(gen);
    height_map[grid_size - 1][grid_size - 1] = dist(gen);

    int step_size = grid_size - 1;
    double scale = roughness;

    while (step_size > 1)
    {
        for (int x = 0; x < grid_size - 1; x += step_size)
        {
            for (int y = 0; y < grid_size - 1; y += step_size)
            {
                diamond_step(x, y, step_size, scale);
            }
        }

        for (int x = 0; x < grid_size - 1; x += step_size)
        {
            for (int y = 0; y < grid_size - 1; y += step_size)
            {
                square_step(x, y, step_size, scale);
            }
        }

        step_size /= 2;
        scale *= roughness;
    }

    generate_peaks();
    generate_mesh();
    // generate_mesh_normals();
}

FractalMountain::~FractalMountain() {}

void FractalMountain::diamond_step(int x, int y, int step_size, double scale)
{
    int half = step_size / 2;
    double avg = (height_map[x][y] +
                  height_map[x + step_size][y] +
                  height_map[x][y + step_size] +
                  height_map[x + step_size][y + step_size]) /
                 4.0;
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-scale, scale);
    height_map[x + half][y + half] = avg + dist(gen);
}

void FractalMountain::square_step(int x, int y, int step_size, double scale)
{
    int half = step_size / 2;
    double avg = 0.0;
    int count = 0;

    if (x - half >= 0)
    {
        avg += height_map[x - half][y];
        count++;
    }
    if (x + half < grid_size)
    {
        avg += height_map[x + half][y];
        count++;
    }
    if (y - half >= 0)
    {
        avg += height_map[x][y - half];
        count++;
    }
    if (y + half < grid_size)
    {
        avg += height_map[x][y + half];
        count++;
    }

    avg /= count;

    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-scale, scale);
    height_map[x][y + half] = avg + dist(gen);
}

void FractalMountain::generate_mesh()
{
    m_vertices.clear();
    m_faces.clear();

    double offset = grid_size / 2.0;

    glm::dvec3 lower_left = glm::dvec3(std::numeric_limits<double>::max());
    glm::dvec3 upper_right = glm::dvec3(std::numeric_limits<double>::min());
    for (int y = 0; y < grid_size; y++)
    {
        for (int x = 0; x < grid_size; x++)
        {
            glm::dvec3 vertex = glm::dvec3(x - offset, height_map[x][y], y - offset);
            m_vertices.push_back(vertex);

            lower_left.x = glm::min(lower_left.x, vertex.x);
            lower_left.y = glm::min(lower_left.y, vertex.y);
            lower_left.z = glm::min(lower_left.z, vertex.z);
            upper_right.x = glm::max(upper_right.x, vertex.x);
            upper_right.y = glm::max(upper_right.y, vertex.y);
            upper_right.z = glm::max(upper_right.z, vertex.z);
        }
    }

    for (size_t y = 0; y < grid_size - 1; ++y)
    {
        for (size_t x = 0; x < grid_size - 1; ++x)
        {
            size_t top_left = y * grid_size + x;
            size_t top_right = y * grid_size + x + 1;
            size_t bottom_left = (y + 1) * grid_size + x;
            size_t bottom_right = (y + 1) * grid_size + x + 1;

            m_faces.push_back({bottom_left, top_right, top_left});
            m_faces.push_back({bottom_left, bottom_right, top_right});
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
    auto center = (upper_right + lower_left) / 2.0;
    bounding_box.translate(center);

    std::cout << "Loaded " << m_vertices.size() << " vertices and " << m_faces.size() << " faces" << std::endl;
}

void FractalMountain::generate_peaks()
{
    for (int y = 0; y < grid_size; ++y)
    {
        for (int x = 0; x < grid_size; ++x)
        {
            double height_change = 0.0;
            for (auto &peak : peaks)
            {
                double distance_x = (x - grid_size / 2) - peak.x;
                double distance_z = (y - grid_size / 2) - peak.z;
                double distance = std::sqrt(distance_x * distance_x + distance_z * distance_z) + 1.0;
                height_change += peak.y / (glm::max(distance / 1.3, 1.0));
            }
            height_map[x][y] += height_change;
        }
    }
}

void FractalMountain::generate_mesh_normals()
{
    m_normals.clear();
    m_normals.resize(m_vertices.size(), glm::dvec3(0.0));

    glm::dvec3 lower_left = glm::dvec3(std::numeric_limits<double>::max());
    glm::dvec3 upper_right = glm::dvec3(std::numeric_limits<double>::min());
    for (auto &face : m_faces)
    {
        glm::dvec3 v0 = m_vertices[face.v1];
        glm::dvec3 v1 = m_vertices[face.v2];
        glm::dvec3 v2 = m_vertices[face.v3];

        lower_left.x = glm::min(lower_left.x, v0.x);
        lower_left.y = glm::min(lower_left.y, v0.y);
        lower_left.z = glm::min(lower_left.z, v0.z);
        lower_left.x = glm::min(lower_left.x, v1.x);
        lower_left.y = glm::min(lower_left.y, v1.y);
        lower_left.z = glm::min(lower_left.z, v1.z);
        lower_left.x = glm::min(lower_left.x, v2.x);
        lower_left.y = glm::min(lower_left.y, v2.y);
        lower_left.z = glm::min(lower_left.z, v2.z);

        upper_right.x = glm::max(upper_right.x, v0.x);
        upper_right.y = glm::max(upper_right.y, v0.y);
        upper_right.z = glm::max(upper_right.z, v0.z);
        upper_right.x = glm::max(upper_right.x, v1.x);
        upper_right.y = glm::max(upper_right.y, v1.y);
        upper_right.z = glm::max(upper_right.z, v1.z);
        upper_right.x = glm::max(upper_right.x, v2.x);
        upper_right.y = glm::max(upper_right.y, v2.y);
        upper_right.z = glm::max(upper_right.z, v2.z);

        glm::dvec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        m_normals[face.v1] += normal;
        m_normals[face.v2] += normal;
        m_normals[face.v3] += normal;
        face.n1 = face.v1;
        face.n2 = face.v2;
        face.n3 = face.v3;
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
    auto center = (upper_right + lower_left) / 2.0;
    bounding_box.translate(center);

    std::cout << "corner " << glm::to_string(corner_distance) << std::endl;
    std::cout << "center " << glm::to_string(center) << std::endl;

    for (auto &normal : m_normals)
    {
        normal = glm::normalize(normal);
    }
}
