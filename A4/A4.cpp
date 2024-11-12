// Termm--Fall 2024

#include <glm/ext.hpp>
#include <iomanip>
#include <iostream>
#include "PhongMaterial.hpp"
#include "A4.hpp"

glm::vec3 get_color(SceneNode *root, std::pair<glm::vec4, glm::vec4> ray, const glm::vec3 &ambient, const std::list<Light *> &lights, int depth, float ior)
{
    auto intersect = root->intersect(ray);
    if (intersect)
    {
        PhongMaterial *mat = (PhongMaterial *)intersect->mat;
        glm::vec3 light_color = ambient * mat->m_kd;
        glm::vec3 refraction_component = glm::vec3(0);
        glm::vec3 normal = glm::normalize(glm::vec3(intersect->normal));
        // if (depth == 2)
        // {
        //     return glm::vec3(1.0);
        // }
        // std::cout << "depth: " << depth << " refraction: " << mat->m_index_of_refraction << " transparency: " << mat->m_transparency << std::endl;
        if (mat->m_index_of_refraction && mat->m_transparency && depth < 6)
        {

            // float eta = ior / mat->m_index_of_refraction;
            // if (ior == mat->m_index_of_refraction)
            // {
            //     eta = ior;
            // }

            // auto etai_over_etat = 1.0 / mat->m_index_of_refraction;
            // if (glm::dot(intersect->point - ray.first, intersect->normal) > 0)
            // {
            //     etai_over_etat = mat->m_index_of_refraction / 1.0;
            // }
            // glm::vec4 uv = glm::normalize(intersect->point - ray.first);
            // auto cos_theta = glm::min((double)glm::dot(-uv, intersect->normal), 1.0);
            // glm::vec4 r_out_perp = (double)etai_over_etat * (uv + cos_theta * intersect->normal);
            // glm::vec4 r_out_parallel = glm::sqrt(glm::abs(1.0 - glm::length2(r_out_perp))) * intersect->normal;
            // auto refraction_ray = r_out_perp + r_out_parallel;
            auto refraction_ray = glm::reflect(intersect->point - ray.first, intersect->normal);
            auto intersect_corrected = intersect->point + 0.01 * refraction_ray;
            refraction_component = get_color(root, std::make_pair(intersect_corrected, intersect->point + refraction_ray), ambient, lights, depth + 1, mat->m_index_of_refraction);
            // std::cout
            //     << 1 / mat->m_index_of_refraction << std::endl;
            // // auto refraction_ray = glm::refract(intersect->point - ray.first, intersect->normal, 1 / mat->m_index_of_refraction);
            // auto correct_intersect = intersect->point + 0.01 * refraction_ray;
            // std::cout << glm::to_string(intersect->point) << std::endl;
            // std::cout << glm::to_string(glm::normalize(refraction_ray)) << std::endl;
            // std::cout << glm::to_string(glm::normalize(intersect->point - ray.first)) << std::endl;
            // if (glm::dot(refraction_ray, intersect->normal) < 0)
            // {
            //     auto exit_point = root->intersect(std::make_pair(correct_intersect, correct_intersect + refraction_ray));
            //     if (!exit_point)
            //     {
            //         return glm::vec3(1.0);
            //     }
            //     std::cout << glm::to_string(exit_point->point) << std::endl;
            //     auto continued_ray = glm::refract(exit_point->point - correct_intersect, -exit_point->normal, mat->m_index_of_refraction);
            //     auto exit_point_corrected = exit_point->point + 0.01 * continued_ray;
            //     refraction_component = get_color(root, std::make_pair(exit_point_corrected, exit_point_corrected + continued_ray), ambient, lights, depth + 1, mat->m_index_of_refraction);
            // }
        }
        // INSERT COLOR CALCULATION HERE
        for (auto light : lights)
        {
            glm::vec3 light_pos = glm::vec3(light->position);
            auto corrected_intersect = intersect->point + 0.01 * intersect->normal;
            glm::vec3 light_dir = glm::normalize(light_pos - glm::vec3(corrected_intersect));
            // light_color = normal;
            // Shadow check
            auto shadow_ray = std::make_pair(corrected_intersect, glm::vec4(light_pos, 1.0));
            auto shadow_intersect = root->intersect(shadow_ray);
            if (
                !shadow_intersect || glm::distance(glm::vec3(shadow_intersect->point), light_pos) > glm::distance(glm::vec3(intersect->point), light_pos))
            { // If there's no intersection, it's not in shadow

                // Diffuse reflection
                double diffuse_intensity = std::max((double)glm::dot(normal, light_dir), 0.0);
                glm::vec3 diffuse_term = mat->m_kd * light->colour * diffuse_intensity;

                // Specular reflection
                glm::vec3 view_dir = glm::normalize(glm::vec3(ray.first - intersect->point));
                glm::vec3 reflect_dir = glm::reflect(-light_dir, normal);
                double specular_intensity = std::pow(std::max((double)glm::dot(view_dir, reflect_dir), 0.0), mat->m_shininess);
                glm::vec3 specular_term = mat->m_ks * light->colour * specular_intensity;

                // Accumulate color
                light_color += diffuse_term + specular_term;
            }
            if (shadow_intersect)
            {
                delete shadow_intersect;
            }
        }
        delete intersect;
        return (1.0 - mat->m_transparency) * light_color + mat->m_transparency * refraction_component;
    }
    else
    {
        auto probability_of_star = 0.005;
        if ((double)rand() / RAND_MAX < probability_of_star)
        {
            return glm::vec3(1.0);
        }
        else
        {
            return glm::vec3(0.0);
        }
    }
}

void A4_Render(
    // What to render
    SceneNode *root,

    // Image to write to, set to a given width and height
    Image &image,

    // Viewing parameters
    const glm::vec3 &eye,
    const glm::vec3 &view,
    const glm::vec3 &up,
    double fovy,

    // Lighting parameters
    const glm::vec3 &ambient,
    const std::list<Light *> &lights)
{

    std::cout << "F24: Calling A4_Render(\n"
              << "\t" << *root << "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
                                                                                                              "\t"
              << "eye:  " << glm::to_string(eye) << std::endl
              << "\t" << "view: " << glm::to_string(view) << std::endl
              << "\t" << "up:   " << glm::to_string(up) << std::endl
              << "\t" << "fovy: " << fovy << std::endl
              << "\t" << "ambient: " << glm::to_string(ambient) << std::endl
              << "\t" << "lights{" << std::endl;

    for (const Light *light : lights)
    {
        std::cout << "\t\t" << *light << std::endl;
    }
    std::cout << "\t}" << std::endl;
    std::cout << ")" << std::endl;

    size_t h = image.height();
    size_t w = image.width();

    double total_pixels = double(h * w);

    double focal_length = 10.0;

    double vh = 2 * std::tan(glm::radians(fovy / 2)) * focal_length;
    double vw = vh * (double(w) / h);

    glm::vec3 v_right = vw * glm::normalize(glm::cross(view - eye, up));
    glm::vec3 v_down = vh * glm::normalize(-up);

    glm::vec3 pixel_delta_x = v_right / w;
    glm::vec3 pixel_delta_y = v_down / h;

    glm::vec3 vp_upper_left = eye + focal_length * glm::normalize(view - eye) - v_right / 2 - v_down / 2;
    glm::vec3 pixel00 = vp_upper_left + 0.5 * (pixel_delta_x + pixel_delta_y);
    for (uint y = 0; y < h; ++y)
    {
        for (uint x = 0; x < w; ++x)
        {

            glm::vec3 pixel = pixel00 + (x * pixel_delta_x) + (y * pixel_delta_y);

            std::pair<glm::vec4, glm::vec4> ray = std::make_pair(glm::vec4(eye, 1), glm::vec4(pixel, 1));
            auto color = get_color(root, ray, ambient, lights, 0, 1.0);
            image(x, y, 0) = color.r;
            image(x, y, 1) = color.g;
            image(x, y, 2) = color.b;
        }
        std::clog << "\rProgress: " << std::setprecision(2) << std::fixed << ((y * w) / total_pixels) * 100 << "% " << std::flush;
    }
    std::clog << "\rDone.                      " << std::endl;
}
