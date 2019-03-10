#pragma once

#include <vector>
#include <glm/glm.hpp>

///////////// Structs that will be transfered to the GPU /////////////

//Alignment: 16 bytes, therefore two vec4 vectors
struct PointLight
{
    glm::vec4 position;
    glm::vec4 color;
    glm::vec4 attenuation;
};

struct DirectionalLight
{
    glm::vec4 direction;
    glm::vec4 color;
    glm::vec4 attenuation;
};
// The number of attributes in the struct. No Reflection yet in C++.
// Will be needed for uploading the scene in the SceneManager.
constexpr unsigned int NumAttributesLights = 3;

//Alignment will be round to 16bytes
struct Sphere
{
    glm::vec3 center;
    float radius;
};

//Alignment will be 16 bytes therefore take two vec4 vectors
struct Plane
{
    glm::vec4 normal;
    glm::vec4 pos;
};

struct Triangle
{
    glm::vec4 A, B, C;
};

struct Material
{
    glm::vec4 diffuse;
    glm::vec4 specularity;
    glm::vec4 emission;
    float shininess;
};
constexpr unsigned int NumAttributesMaterial = 4;

struct Primitive
{
    Triangle t;
    int type;
    int material_index;
};
constexpr unsigned int NumAttributesObjects = 5;

struct Scene
{
    std::vector<glm::vec3> vertices;
    std::vector<std::pair<Triangle, int>> triangles;
    std::vector<std::pair<Sphere, int>> spheres;
    std::vector<std::pair<Plane, int>> planes;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<Material> materials;

    int numObjects()
    {
        return triangles.size() + spheres.size() + planes.size();
    }

    int numLights()
    {
        return pointLights.size() + directionalLights.size();
    }

    void clear()
    {
        vertices.clear();
        triangles.clear();
        spheres.clear();
        planes.clear();
        pointLights.clear();
        directionalLights.clear();
        materials.clear();
    }
};