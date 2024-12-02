// Termm--Fall 2024

#include <glm/ext.hpp>
#include <iomanip>
#include <iostream>
#include "PhongMaterial.hpp"
#include "A4.hpp"

glm::dvec3 get_background_color(const glm::dvec3 &up, std::pair<glm::dvec3, glm::dvec3> ray)
{

    glm::dvec3 start_color = glm::dvec3(59, 222, 255) / 255.0; // glm::dvec3(231, 154, 255) / 255.0;
    glm::dvec3 end_color = glm::dvec3(255, 59, 167) / 255.0;   // glm::dvec3(4, 21, 120) / 255.0;
    double lerp_param = (glm::dot(up, glm::normalize(ray.second - ray.first)) + 1) / 2;
    if (lerp_param > 0.5)
    {
        lerp_param = 1 - glm::pow(-2 * lerp_param + 2, 5) / 2;
    }
    else
    {
        lerp_param = 16 * glm::pow(lerp_param, 5);
    }
    return glm::mix(end_color, start_color, lerp_param);
}

glm::dvec3 get_refract_ray(Intersection *intersect, glm::dvec3 &origin)
{
    PhongMaterial *mat = (PhongMaterial *)intersect->mat;
    double n1 = 1.0;
    double n2 = mat->m_index_of_refraction;

    double eta = 1.0 / mat->m_index_of_refraction;

    if (glm::dot(intersect->point - origin, intersect->normal) > 0)
    {
        n1 = mat->m_index_of_refraction;
        n2 = 1.0;
        eta = 1.0 / eta;
        intersect->normal = -intersect->normal;
    }
    return glm::refract(glm::normalize(intersect->point - origin), glm::normalize(intersect->normal), eta);
}

glm::dvec3 get_refract_color(
    Intersection *intersect,
    SceneNode *root,
    std::pair<glm::dvec3, glm::dvec3> ray,
    const glm::dvec3 &ambient,
    const std::list<Light *> &lights,
    int depth,
    const glm::dvec3 &up)
{
    PhongMaterial *mat = (PhongMaterial *)intersect->mat;
    glm::dvec3 refraction_component = glm::dvec3(0);
    glm::dvec3 reflection_component = glm::dvec3(0);
    double n1 = 1.0;
    double n2 = mat->m_index_of_refraction;
    bool beers_law = true;
    double eta = 1.0 / mat->m_index_of_refraction;

    if (glm::dot(intersect->point - ray.first, intersect->normal) > 0)
    {
        n1 = mat->m_index_of_refraction;
        n2 = 1.0;
        eta = 1.0 / eta;
        intersect->normal = -intersect->normal;
        beers_law = false;
    }

    double R0 = glm::pow((n1 - n2) / (n1 + n2), 2);
    double R = R0 + (1 - R0) * glm::pow(1 - glm::dot(glm::normalize(ray.first - intersect->point), glm::normalize(intersect->normal)), 5);

    auto refraction_ray = glm::refract(glm::normalize(intersect->point - ray.first), glm::normalize(intersect->normal), eta);
    auto reflection_ray = glm::reflect(glm::normalize(intersect->point - ray.first), glm::normalize(intersect->normal));
    auto intersect_corrected = intersect->point + 0.001 * refraction_ray;
    double epsilon = 0.01;

    if (R > 1.0 - epsilon || (mat->m_reflectivity == 1.0 && mat->m_transparency == 0.0))
        R = 1.0;
    if (R < epsilon)
        R = 0.0;

    if (R < 1.0 && !glm::all(glm::isnan(refraction_ray)))
    {
        auto refract_ray = std::make_pair(intersect_corrected, intersect->point + refraction_ray);
        if (mat->m_transparency_glossiness && depth <= 1)
        {
            auto distant_circle_center = intersect->point + glm::normalize(refraction_ray);
            auto distant_circle_radius = glm::atan(glm::radians(mat->m_transparency_glossiness) / 2);
            auto normal = intersect->normal;
            glm::dvec3 tangent = glm::dvec3(0);
            glm::dvec3 bitangent = glm::dvec3(0);
            if (glm::abs(refraction_ray.x) > glm::abs(refraction_ray.z))
            {
                tangent = glm::dvec3(-refraction_ray.y, refraction_ray.x, 0);
            }
            else
            {
                tangent = glm::dvec3(0, -refraction_ray.z, refraction_ray.y);
            }
            bitangent = glm::cross(refraction_ray, tangent);
            glm::dvec3 refract_color = glm::dvec3(0);
            double rays_to_cast = 20.0;
            for (int i = 0; i < rays_to_cast; ++i)
            {
                double radius = rand() % 1000 / 1000.0 * distant_circle_radius;
                double angle = rand() % 1000 / 1000.0 * 2 * M_PI;

                glm::dvec3 offset = radius * (glm::cos(angle) * tangent + glm::sin(angle) * bitangent) + distant_circle_center;

                refract_ray = std::make_pair(intersect_corrected, offset);
                refract_color += get_color(
                    root,
                    refract_ray,
                    ambient,
                    lights,
                    depth + 1,
                    up);
            }
            refraction_component = refract_color / rays_to_cast;
        }
        else
        {
            refraction_component = get_color(
                root,
                refract_ray,
                ambient,
                lights,
                depth + 1,
                up);
        }
        auto refract_intersect = root->intersect(refract_ray);
        if (beers_law && refract_intersect)
        {
            refraction_component *= glm::exp(-mat->m_absorption * glm::distance(intersect->point, refract_intersect->point));
        }
    }
    else
    {
        refraction_component = get_background_color(up, ray);
    }
    if (R > 0 && !glm::all(glm::isnan(reflection_ray)))
    {
        if (mat->m_reflectivity_glossiness && depth <= 1)
        {
            auto distant_circle_center = intersect->point + glm::normalize(reflection_ray);
            auto distant_circle_radius = glm::atan(mat->m_reflectivity_glossiness);
            glm::dvec3 tangent = glm::dvec3(0);
            glm::dvec3 bitangent = glm::dvec3(0);
            if (glm::abs(reflection_ray.x) > glm::abs(reflection_ray.z))
            {
                tangent = glm::dvec3(-reflection_ray.y, reflection_ray.x, 0);
            }
            else
            {
                tangent = glm::dvec3(0, -reflection_ray.z, reflection_ray.y);
            }
            bitangent = glm::cross(reflection_ray, tangent);
            glm::dvec3 refract_color = glm::dvec3(0);
            double rays_to_cast = 20.0;
            for (int i = 0; i < rays_to_cast; ++i)
            {
                double radius = (rand() % 1000) / 1000.0 * distant_circle_radius;
                double angle = (rand() % 1000) / 1000.0 * 2 * M_PI;

                glm::dvec3 offset = radius * (glm::cos(angle) * tangent + glm::sin(angle) * bitangent);
                intersect_corrected = intersect->point + 0.001 * reflection_ray;
                auto reflect_ray = std::make_pair(intersect_corrected, distant_circle_center + offset);
                refract_color += get_color(
                    root,
                    reflect_ray,
                    ambient,
                    lights,
                    depth + 1,
                    up);
            }
            reflection_component = refract_color / rays_to_cast;
        }
        else
        {
            intersect_corrected = intersect->point + 0.001 * reflection_ray;
            reflection_component = get_color(
                root,
                std::make_pair(intersect_corrected, intersect->point + reflection_ray),
                ambient,
                lights,
                depth + 1,
                up);
        }
    }
    else
    {
        reflection_component = get_background_color(up, ray);
    }
    return mat->m_transparency * refraction_component * (1.0 - R) + mat->m_reflectivity * reflection_component * R;
}

glm::dvec3 get_color(
    SceneNode *root,
    std::pair<glm::dvec3, glm::dvec3> ray,
    const glm::dvec3 &ambient,
    const std::list<Light *> &lights,
    int depth,
    const glm::dvec3 &up)
{
    auto sky_blue = get_background_color(up, ray);
    auto intersect = root->intersect(ray);
    if (intersect)
    {
        if (intersect->edge_hit)
        {
            // delete intersect;
            // return glm::dvec3(0, 1, 1);
        }
        PhongMaterial *mat = (PhongMaterial *)intersect->mat;
        glm::dvec3 ambient_color = ambient * mat->m_kd;
        glm::dvec3 refraction_component = glm::dvec3(0);
        glm::dvec3 normal = glm::normalize(glm::dvec3(intersect->normal));

        if (((mat->m_index_of_refraction && mat->m_transparency) || mat->m_reflectivity) && depth < 4)
        {
            refraction_component = get_refract_color(intersect, root, ray, ambient, lights, depth, up);
        }
        else if (depth >= 4)
        {
            refraction_component = sky_blue;
        }
        // INSERT COLOR CALCULATION HERE
        glm::dvec3 light_color = ambient_color;
        glm::dvec3 diffuse_term = glm::dvec3(0);
        glm::dvec3 specular_term = glm::dvec3(0);
        for (auto light : lights)
        {
            glm::dvec3 light_pos = light->position;
            auto corrected_intersect = intersect->point + 0.01 * intersect->normal;
            glm::dvec3 light_dir = glm::normalize(light_pos - corrected_intersect);
            auto shadow_ray = std::make_pair(corrected_intersect, light_pos);
            auto shadow_intersect = root->intersect(shadow_ray, true);
            if (
                !shadow_intersect || glm::distance(shadow_intersect->point, intersect->point) > glm::distance(glm::dvec3(intersect->point), light_pos))
            {

                double distance_to_light = glm::distance(intersect->point, light->position);
                double attenuation_value = 1 / (light->falloff[0] + light->falloff[1] * (distance_to_light) + light->falloff[2] * (distance_to_light * distance_to_light));

                double diffuse_intensity = std::max((double)glm::dot(normal, light_dir), 0.0);
                light_color += mat->m_kd * attenuation_value * light->colour * diffuse_intensity;
                glm::dvec3 view_dir = glm::normalize(glm::dvec3(ray.first - intersect->point));
                glm::dvec3 reflect_dir = glm::reflect(-light_dir, normal);
                double specular_intensity = std::pow(std::max((double)glm::dot(view_dir, reflect_dir), 0.0), mat->m_shininess);
                light_color += mat->m_ks * attenuation_value * light->colour * specular_intensity;
            }
            if (shadow_intersect)
            {
                delete shadow_intersect;
            }
        }
        delete intersect;
        glm::dvec3 ret_color = (1.0 - mat->m_transparency) * (1.0 - mat->m_reflectivity) * light_color + refraction_component + mat->m_ke;
        if (depth == 0)
        {
            ret_color.x = glm::clamp(ret_color.x, 0.0, 1.0);
            ret_color.y = glm::clamp(ret_color.y, 0.0, 1.0);
            ret_color.z = glm::clamp(ret_color.z, 0.0, 1.0);
        }
        return ret_color;
    }
    else
    {
        return sky_blue;
    }
}

glm::dvec3 get_color(Image &image, size_t x, size_t y)
{
    glm::dvec3 color = glm::dvec3(0);
    color.r = image(x, y, 0);
    color.g = image(x, y, 1);
    color.b = image(x, y, 2);
    return color;
}

bool should_anti_alias(Image &image, size_t x, size_t y, double threshold, size_t width, size_t height)
{
    glm::dvec3 color = get_color(image, x, y);
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            if (x + i < 0 || y + j < 0 || x + i >= width || y + j >= height)
            {
                continue;
            }
            glm::dvec3 new_color = get_color(image, x + i, y + j);
            if (glm::distance(color, new_color) > threshold)
            {
                return true;
            }
        }
    }
    return false;
}

void A4_Render(
    // What to render
    SceneNode *root,

    // Image to write to, set to a given width and height
    Image &image,

    // Viewing parameters
    const glm::dvec3 &eye,
    const glm::dvec3 &view,
    const glm::dvec3 &up,
    double fovy,

    // Lighting parameters
    const glm::dvec3 &ambient,
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

    glm::dvec3 v_right = vw * glm::normalize(glm::cross(view - eye, up));
    glm::dvec3 v_down = vh * glm::normalize(-up);

    glm::dvec3 pixel_delta_x = v_right / w;
    glm::dvec3 pixel_delta_y = v_down / h;

    glm::dvec3 vp_upper_left = eye + focal_length * glm::normalize(view - eye) - v_right / 2.0 - v_down / 2.0;
    glm::dvec3 pixel00 = vp_upper_left + 0.5 * (pixel_delta_x + pixel_delta_y);
    glm::dvec3 forward = glm::normalize(view - eye);
    glm::dvec3 up_corrected = glm::normalize(up - glm::dot(up, forward) * forward);
    for (uint y = 0; y < h; ++y)
    {
        for (uint x = 0; x < w; ++x)
        {

            glm::dvec3 pixel = pixel00 + (double(x) * pixel_delta_x) + (double(y) * pixel_delta_y);

            std::pair<glm::dvec3, glm::dvec3> ray = std::make_pair(eye, pixel);
            auto color = get_color(root, ray, ambient, lights, 0, up_corrected);

            image(x, y, 0) = color.r;
            image(x, y, 1) = color.g;
            image(x, y, 2) = color.b;
        }
        std::clog << "\rProgress: " << std::setprecision(2) << std::fixed << ((y * w) / total_pixels) * 100 << "% " << std::flush;
    }
    Image unaliased_image = image;
    std::clog << "\rDone. On to Anti-Aliasing               " << std::endl;
    double threshold = 0.1;
    for (uint y = 0; y < h; ++y)
    {
        // for (uint x = 0; x < w; ++x)
        // {
        //     if (should_anti_alias(unaliased_image, x, y, threshold, w, h))
        //     {
        //         glm::dvec3 new_color = glm::dvec3(0);
        //         for (int p_count = 0; p_count < 20; ++p_count)
        //         {
        //             glm::dvec3 pixel = pixel00 + (double(x) * pixel_delta_x) + (double(y) * pixel_delta_y);
        //             double x_jitter = ((rand() % 100) - 100) / 100.0;
        //             double y_jitter = ((rand() % 100) - 100) / 100.0;
        //             pixel += x_jitter * pixel_delta_x + y_jitter * pixel_delta_y;
        //             std::pair<glm::dvec3, glm::dvec3> ray = std::make_pair(eye, pixel);
        //             auto color = get_color(root, ray, ambient, lights, 0, up);
        //             new_color += color;
        //         }
        //         new_color /= 20.0;
        //         image(x, y, 0) = new_color.r;
        //         image(x, y, 1) = new_color.g;
        //         image(x, y, 2) = new_color.b;
        //     }
        // }
        std::clog << "\rProgress: " << std::setprecision(2) << std::fixed << ((y * w) / total_pixels) * 100 << "% " << std::flush;
    }
    std::clog << "\rDone. For reals             " << std::endl;
}
