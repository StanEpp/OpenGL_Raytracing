#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Primitives.h"
#include "Lights.h"

struct Scene{
    std::vector<glm::vec3> vertices;
    std::vector<std::pair<Triangle, int>> triangles;
    std::vector<std::pair<Sphere, int>> spheres;
    std::vector<std::pair<Plane, int>> planes;
    std::vector<PointLight> pointLights;
    std::vector<DirectionalLight> directionalLights;
    std::vector<Material> materials;

    int numberOfObjects()
    {
        return triangles.size() + spheres.size() + planes.size();
    }

    int numberOfLights()
    {
        return pointLights.size() + directionalLights.size();
    }

    void clear() {
        vertices.clear();
        triangles.clear();
        spheres.clear();
        planes.clear();
        pointLights.clear();
        directionalLights.clear();
        materials.clear();
    }
};