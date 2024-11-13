// Termm--Fall 2024

#include <glm/ext.hpp>
#include <iomanip>
#include <iostream>
#include "PhongMaterial.hpp"
#include "A4.hpp"

glm::dvec3 get_background_color(const glm::dvec3 &up, std::pair<glm::dvec3, glm::dvec3> ray)
{

    glm::dvec3 start_color = glm::dvec3(231, 154, 255) / 255.0;
    glm::dvec3 end_color = glm::dvec3(193.0 / 255.0, 9.0 / 255.0, 0);
    double lerp_param = (glm::dot(up, glm::normalize(ray.second - ray.first)) + 1) / 2;
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

    double eta = 1.0 / mat->m_index_of_refraction;

    if (glm::dot(intersect->point - ray.first, intersect->normal) > 0)
    {
        n1 = mat->m_index_of_refraction;
        n2 = 1.0;
        eta = 1.0 / eta;
        intersect->normal = -intersect->normal;
    }

    double R0 = glm::pow((n1 - n2) / (n1 + n2), 2);
    double R = R0 + (1 - R0) * glm::pow(1 - glm::dot(glm::normalize(ray.first - intersect->point), glm::normalize(intersect->normal)), 5);

    auto refraction_ray = glm::refract(glm::normalize(intersect->point - ray.first), glm::normalize(intersect->normal), eta);
    auto reflection_ray = glm::reflect(glm::normalize(intersect->point - ray.first), glm::normalize(intersect->normal));
    auto intersect_corrected = intersect->point + 0.001 * refraction_ray;
    double epsilon = 0.00001;

    if (R > 1.0 - epsilon)
        R = 1.0;
    if (R < epsilon)
        R = 0.0;

    if (R < 1.0 && !glm::all(glm::isnan(refraction_ray)))
    {
        refraction_component = get_color(
            root,
            std::make_pair(intersect_corrected, intersect->point + refraction_ray),
            ambient,
            lights,
            depth + 1,
            up);
    }
    else
    {
        refraction_component = get_background_color(up, ray);
    }
    if (R > 0)
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
    else
    {
        reflection_component = get_background_color(up, ray);
    }
    return refraction_component * (1.0 - R) + reflection_component * R;
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
        PhongMaterial *mat = (PhongMaterial *)intersect->mat;
        glm::dvec3 ambient_color = ambient * mat->m_kd;
        glm::dvec3 refraction_component = glm::dvec3(0);
        glm::dvec3 normal = glm::normalize(glm::dvec3(intersect->normal));
        if (mat->m_index_of_refraction && mat->m_transparency && depth < 4)
        {
            refraction_component = get_refract_color(intersect, root, ray, ambient, lights, depth, up);
        }
        else
        {
            refraction_component = sky_blue;
        }
        // INSERT COLOR CALCULATION HERE
        glm::dvec3 diffuse_term = glm::dvec3(0);
        glm::dvec3 specular_term = glm::dvec3(0);
        for (auto light : lights)
        {
            glm::dvec3 light_pos = light->position;
            auto corrected_intersect = intersect->point + 0.01 * intersect->normal;
            glm::dvec3 light_dir = glm::normalize(light_pos - corrected_intersect);
            auto shadow_ray = std::make_pair(corrected_intersect, light_pos);
            auto shadow_intersect = root->intersect(shadow_ray);
            if (shadow_intersect) // no shadows if the object is transparent
            {
                PhongMaterial *shadow_mat = (PhongMaterial *)shadow_intersect->mat;
                if (shadow_mat->m_transparency)
                {
                    shadow_intersect = nullptr;
                }
            }
            if (
                !shadow_intersect || glm::distance(shadow_intersect->point, intersect->point) > glm::distance(glm::dvec3(intersect->point), light_pos))
            {

                double distance_to_light = glm::distance(intersect->point, light->position);
                double attenuation_value = 1 / (light->falloff[0] + light->falloff[1] * (distance_to_light) + light->falloff[2] * (distance_to_light * distance_to_light));

                double diffuse_intensity = std::max((double)glm::dot(normal, light_dir), 0.0);
                diffuse_term += mat->m_kd * attenuation_value * light->colour * diffuse_intensity;
                glm::dvec3 view_dir = glm::normalize(glm::dvec3(ray.first - intersect->point));
                glm::dvec3 reflect_dir = glm::reflect(-light_dir, normal);
                double specular_intensity = std::pow(std::max((double)glm::dot(view_dir, reflect_dir), 0.0), mat->m_shininess);
                specular_term += mat->m_ks * attenuation_value * light->colour * specular_intensity;
            }
            if (shadow_intersect)
            {
                delete shadow_intersect;
            }
        }
        delete intersect;
        glm::dvec3 ret_color = (1.0 - mat->m_transparency) * diffuse_term + specular_term + refraction_component * mat->m_transparency + ambient_color;
        ret_color.x = glm::clamp(ret_color.x, 0.0, 1.0);
        ret_color.y = glm::clamp(ret_color.y, 0.0, 1.0);
        ret_color.z = glm::clamp(ret_color.z, 0.0, 1.0);
        return ret_color;
    }
    else
    {
        return sky_blue;
    }
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
    for (uint y = 0; y < h; ++y)
    {
        for (uint x = 0; x < w; ++x)
        {

            glm::dvec3 pixel = pixel00 + (double(x) * pixel_delta_x) + (double(y) * pixel_delta_y);

            std::pair<glm::dvec3, glm::dvec3> ray = std::make_pair(eye, pixel);
            auto color = get_color(root, ray, ambient, lights, 0, up);

            image(x, y, 0) = color.r;
            image(x, y, 1) = color.g;
            image(x, y, 2) = color.b;
        }
        std::clog << "\rProgress: " << std::setprecision(2) << std::fixed << ((y * w) / total_pixels) * 100 << "% " << std::flush;
    }
    std::clog << "\rDone.                      " << std::endl;
}
