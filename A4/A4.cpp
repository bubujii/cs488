// Termm--Fall 2024

#include <glm/ext.hpp>
#include "PhongMaterial.hpp"
#include "A4.hpp"

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

    double focal_length = 1.0;

    double vh = 2 * std::tan(glm::radians(fovy / 2)) * focal_length;
    double vw = vh * (double(w) / h);

    glm::vec3 v_right = glm::vec3(vw, 0.0, 0.0);
    glm::vec3 v_down = glm::vec3(0.0, -vh, 0.0);

    glm::vec3 pixel_delta_x = v_right / w;
    glm::vec3 pixel_delta_y = v_down / h;

    glm::vec3 vp_upper_left = eye + focal_length * glm::normalize(view - eye) - v_right / 2 - v_down / 2;
    glm::vec3 pixel00 = vp_upper_left + 0.5 * (pixel_delta_x + pixel_delta_y);
    for (uint y = 0; y < h; ++y)
    {
        for (uint x = 0; x < w; ++x)
        {
            std::clog << "\rProgress: " << (((y * w) + x) / total_pixels) * 100 << "% " << std::flush;

            glm::vec3 pixel = pixel00 + (x * pixel_delta_x) + (y * pixel_delta_y);

            std::pair<glm::vec4, glm::vec4> ray = std::make_pair(glm::vec4(eye, 0), glm::vec4(pixel, 0));
            // std::cout << glm::to_string(ray.first) << std::endl;
            auto intersect = root->intersect(ray);

            if (intersect)
            {
                glm::vec3 light_color;
                PhongMaterial *mat = (PhongMaterial *)intersect->mat;
                glm::vec3 normal = glm::normalize(glm::vec3(intersect->normal) + glm::vec3(1));
                // for (auto light : lights)
                // {
                //     auto light_pos = glm::vec4(light->position, 0);

                //     if (!root->intersect(std::make_pair(intersect->point, light_pos)))
                //     {

                //         // glm::vec3 light_dir = glm::normalize(glm::vec3(light_pos - intersect->point));
                //         // glm::vec3 view_dir = glm::normalize(glm::vec3(eye - glm::vec3(intersect->point)));
                //         // glm::vec3 reflect_dir = glm::normalize(glm::reflect(-light_dir, normal));

                //         // double diffuse = glm::max(double(glm::dot(normal, light_dir)), 0.0);
                //         // double specular = glm::pow(glm::max(double(glm::dot(reflect_dir, view_dir)), 0.0), mat->m_shininess);

                //         // glm::vec3 diffuse_term = mat->m_kd * light->colour * diffuse;
                //         // glm::vec3 specular_term = mat->m_ks * light->colour * specular;

                //         // light_color += diffuse_term + specular_term;
                //     }
                // }
                // glm::vec3 ambient_term = mat->m_kd * ambient;
                // light_color += ambient_term;

                // Red:
                image(x, y, 0) = double(normal.r);
                // Green:
                image(x, y, 1) = double(normal.g);
                // Blue:
                image(x, y, 2) = double(normal.b);

                // PhongMaterial *mat = (PhongMaterial *)intersect->mat;
                // image(x, y, 0) = (double)mat->m_kd.r;
                // // Green:
                // image(x, y, 1) = (double)mat->m_kd.g;
                // // Blue:
                // image(x, y, 2) = (double)mat->m_kd.b;
            }
            else
            {
                // Red:
                image(x, y, 0) = double(0);
                // Green:
                image(x, y, 1) = double(0);
                // Blue:
                image(x, y, 2) = double(0);
            }
        }
    }
    std::clog << "\rDone.                      " << std::endl;
}
