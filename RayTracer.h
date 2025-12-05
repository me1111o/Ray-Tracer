#ifndef RAY_TRACER_H
#define RAY_TRACER_H

#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include <string>

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    std::string textureName;
    float shininess;
    float absorption;
    float reflection;
    float transparency;
};

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct HitRecord
{
    float t;
    glm::vec3 point;
    glm::vec3 normal;
    Material material;
    bool hit;
    glm::vec2 texCoord;
};

class Hittable
{
public:
    virtual bool intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const = 0;
};

class Sphere : public Hittable
{
public:
    glm::vec3 center;
    float radius;
    Material material;

    Sphere(const glm::vec3 &c, float r, const Material &m)
        : center(c), radius(r), material(m) {}

    virtual bool intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const;
};

class Box : public Hittable
{
public:
    glm::vec3 min;
    glm::vec3 max;
    Material material;

    Box(const glm::vec3 &min_, const glm::vec3 &max_, const Material &m)
        : min(min_), max(max_), material(m) {}

    virtual bool intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const;
};

class Cylinder : public Hittable
{
public:
    glm::vec3 center;
    float radius;
    float height;
    Material material;

    Cylinder(const glm::vec3 &c, float r, float h, const Material &m)
        : center(c), radius(r), height(h), material(m) {}

    virtual bool intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const;
};

class Cone : public Hittable
{
public:
    glm::vec3 center;
    float radius;
    float height;
    Material material;

    Cone(const glm::vec3 &c, float r, float h, const Material &m)
        : center(c), radius(r), height(h), material(m) {}

    virtual bool intersect(const Ray &ray, float t_min, float t_max, HitRecord &rec) const;
};

struct Light
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float spotCutoff;
    glm::vec3 spotDirection;

    Light() : spotCutoff(180.0f), spotDirection(glm::vec3(0.0f, -1.0f, 0.0f)) {}
};

glm::vec3 traceRay(const Ray &ray,
                   const std::vector<Hittable *> &objects,
                   const std::vector<Light> &lights,
                   int bounce);

#endif
