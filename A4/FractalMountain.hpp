// #pragma once

// #include "Mesh.hpp"
// #include <random>

// class FractalMountain : public Mesh
// {
// public:
//     FractalMountain();
//     ~FractalMountain();

// private:
//     void subdivide();
//     std::vector<glm::dvec3> peaks;
//     double get_height_mod(glm::dvec3 &point);
//     std::default_random_engine rand_engine = std::default_random_engine();
//     std::normal_distribution<double> rand_gen = std::normal_distribution<double>(0.1, 0.1);
// };

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
