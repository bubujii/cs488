#pragma once

#include "Mesh.hpp"
#include <random>
#include <unordered_map>
#include <glm/glm.hpp>
#include <vector>

class FractalMountain : public Mesh
{
public:
    FractalMountain(int grid_size, double roughness);
    void generate();
    ~FractalMountain();

private:
    int grid_size;
    double roughness;
    std::vector<glm::dvec3> peaks;
    std::vector<std::vector<double>> height_map;

    void diamond_step(int x, int y, int step_size, double scale);
    void square_step(int x, int y, int step_size, double scale);

    void generate_mesh();
    void generate_peaks();
    void generate_mesh_normals();
};
