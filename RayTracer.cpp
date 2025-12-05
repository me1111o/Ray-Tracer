#include "RayTracer.h"
#include <cmath>
#include <algorithm>
#include "TextureLoader.h"
#ifndef M_PI
#define M_PI 3.1415926
#endif

bool Sphere::intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const
{
    glm::vec3 oc = ray.origin - center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - radius * radius;
    float discriminant = b * b - a * c;

    float theta = acos(-rec.normal.y);
    float phi = atan2(-rec.normal.z, rec.normal.x) + M_PI;
    rec.texCoord = glm::vec2(phi / (2.0f * M_PI), theta / M_PI);

    if (discriminant > 0)
    {
        float sqrt_disc = sqrt(discriminant);
        float temp = (-b - sqrt_disc) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.point = ray.origin + rec.t * ray.direction;
            rec.normal = (rec.point - center) / radius;
            rec.material = material;
            rec.hit = true;
            return true;
        }
        temp = (-b + sqrt_disc) / a;
        if (temp < t_max && temp > t_min)
        {
            rec.t = temp;
            rec.point = ray.origin + rec.t * ray.direction;
            rec.normal = (rec.point - center) / radius;
            rec.material = material;
            rec.hit = true;
            return true;
        }
    }
    return false;
}

bool Box::intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const
{
    float t0 = t_min;
    float t1 = t_max;
    glm::vec3 normal;
    for (int i = 0; i < 3; i++)
    {
        float invD = 1.0f / ray.direction[i];
        float tNear = (min[i] - ray.origin[i]) * invD;
        float tFar = (max[i] - ray.origin[i]) * invD;
        glm::vec3 n = glm::vec3(0.0f);
        if (invD < 0)
        {
            std::swap(tNear, tFar);
            n[i] = 1.0f;
        }
        else
        {
            n[i] = -1.0f;
        }
        if (tNear > t0)
        {
            t0 = tNear;
            normal = n;
        }
        t1 = std::min(t1, tFar);
        if (t0 > t1)
            return false;
    }
    if (t0 < t_max && t0 > t_min)
    {
        rec.t = t0;
        rec.point = ray.origin + rec.t * ray.direction;
        rec.normal = normal;
        rec.material = material;
        rec.hit = true;
        return true;
    }
    return false;
}

bool Cylinder::intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const
{
    glm::vec3 oc = ray.origin - center;
    float a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
    float b = 2.0f * (oc.x * ray.direction.x + oc.z * ray.direction.z);
    float c = oc.x * oc.x + oc.z * oc.z - radius * radius;
    float discriminant = b * b - 4 * a * c;
    if (discriminant > 0)
    {
        float sqrt_disc = sqrt(discriminant);
        for (int sign = -1; sign <= 1; sign += 2)
        {
            float temp = (-b + sign * sqrt_disc) / (2.0f * a);
            if (temp < t_max && temp > t_min)
            {
                glm::vec3 point = ray.origin + temp * ray.direction;
                float y = point.y;

                if (y >= center.y && y <= center.y + height)
                {
                    rec.t = temp;
                    rec.point = point;
                    rec.normal = glm::normalize(glm::vec3(point.x - center.x, 0.0f, point.z - center.z));
                    rec.material = material;
                    rec.hit = true;
                    return true;
                }
            }
        }
    }
    glm::vec3 caps[] = {center, center + glm::vec3(0, height, 0)};
    glm::vec3 normals[] = {glm::vec3(0, -1, 0), glm::vec3(0, 1, 0)};

    for (int i = 0; i < 2; i++)
    {
        float denom = glm::dot(ray.direction, normals[i]);
        if (fabs(denom) > 1e-6f)
        {
            float t = glm::dot(caps[i] - ray.origin, normals[i]) / denom;
            if (t > t_min && t < t_max)
            {
                glm::vec3 p = ray.origin + t * ray.direction;
                glm::vec3 v = p - caps[i];
                if (glm::dot(v, v) <= radius * radius)
                {
                    rec.t = t;
                    rec.point = p;
                    rec.normal = normals[i];
                    rec.material = material;
                    rec.hit = true;
                    return true;
                }
            }
        }
    }
    return false;
}

bool Cone::intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const
{
    glm::vec3 co = ray.origin - center;
    float k = radius / height;
    float k2 = k * k;
    float dx = ray.direction.x;
    float dy = ray.direction.y;
    float dz = ray.direction.z;
    float ox = co.x;
    float oy = co.y;
    float oz = co.z;
    float a = dx * dx + dz * dz - k2 * dy * dy;
    float b = 2 * (dx * ox + dz * oz - k2 * dy * (oy));
    float c = ox * ox + oz * oz - k2 * oy * oy;
    float discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return false;
    float sqrt_disc = sqrt(discriminant);
    float t1 = (-b - sqrt_disc) / (2 * a);
    float t2 = (-b + sqrt_disc) / (2 * a);
    if (t1 > t2)
        std::swap(t1, t2);
    for (float t : {t1, t2})
    {
        if (t < t_min || t > t_max)
            continue;
        glm::vec3 point = ray.origin + t * ray.direction;
        float y = point.y - center.y;
        if (y >= 0 && y <= height)
        {
            glm::vec3 v = point - center;
            // slope
            v.y = -radius / height;
            glm::vec3 normal = glm::normalize(v);
            rec.t = t;
            rec.point = point;
            rec.normal = normal;
            rec.material = material;
            rec.hit = true;
            return true;
        }
    }

    glm::vec3 normal = glm::vec3(0, -1, 0);
    float denom = glm::dot(ray.direction, normal);
    if (fabs(denom) > 1e-6f)
    {
        float t = glm::dot(center - ray.origin, normal) / denom;
        if (t > t_min && t < t_max)
        {
            glm::vec3 p = ray.origin + t * ray.direction;
            glm::vec3 v = p - center;
            glm::vec3 viewDir = glm::normalize(ray.origin - p);
            float angle = glm::dot(viewDir, normal);
            if (angle < 0.1f)
                return false;

            if (glm::dot(v, v) <= radius * radius)
            {
                rec.t = t;
                rec.point = p;
                rec.normal = normal;
                rec.material = material;
                rec.hit = true;
                return true;
            }
        }
    }
    return false;
}

glm::vec3 traceRay(const Ray &ray,
                   const std::vector<Hittable *> &objects,
                   const std::vector<Light> &lights,
                   int bounce)
{

    if (bounce <= 0)
        return glm::vec3(0.0f);

    HitRecord closestRec;
    closestRec.t = std::numeric_limits<float>::max();
    closestRec.hit = false;

    for (const auto &obj : objects)
    {
        HitRecord tempRec;
        if (obj->intersect(ray, 0.001f, closestRec.t, tempRec))
            closestRec = tempRec;
    }

    if (!closestRec.hit)
        return glm::vec3(0.0f);

    glm::vec3 norm = glm::normalize(closestRec.normal);
    glm::vec3 viewDir = glm::normalize(-ray.direction);

    glm::vec3 directColor(0.0f);

    const float bias = 1e-4f;
    glm::vec3 shadowOrigin = closestRec.point + bias * norm;

    for (const auto &light : lights)
    {

        glm::vec3 ambient = light.ambient * closestRec.material.ambient;
        glm::vec3 lightDir = glm::normalize(light.position - closestRec.point);

        float diff = std::max(glm::dot(norm, lightDir), 0.0f);
        glm::vec3 diffuse = light.diffuse * (diff * closestRec.material.diffuse);

        glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
        float spec = pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), closestRec.material.shininess);
        glm::vec3 specular = light.specular * (spec * closestRec.material.specular);

        bool inShadow = false;
        Ray shadowRay;
        shadowRay.origin = shadowOrigin;
        shadowRay.direction = lightDir;
        float lightDist = glm::length(light.position - closestRec.point);
        for (const auto &obj : objects)
        {
            HitRecord shadowRec;
            if (obj->intersect(shadowRay, 0.001f, lightDist, shadowRec))
            {
                inShadow = true;
                break;
            }
        }

        if (inShadow)
            directColor += ambient;
        else
            directColor += ambient + 0.8f * (diffuse + specular);
    }
    directColor = glm::clamp(directColor, glm::vec3(0.0f), glm::vec3(1.0f));

    if (closestRec.material.reflection > 0.0f)
    {

        glm::vec3 reflectDir = glm::reflect(ray.direction, norm);
        Ray reflectRay;
        reflectRay.origin = closestRec.point + bias * norm;
        reflectRay.direction = glm::normalize(reflectDir);

        glm::vec3 reflectColor = traceRay(reflectRay, objects, lights, bounce - 1);

        float absorption = 1.0f - closestRec.material.reflection;
        directColor = absorption * directColor + closestRec.material.reflection * reflectColor;
    }
    return glm::clamp(directColor, glm::vec3(0.0f), glm::vec3(1.0f));
}