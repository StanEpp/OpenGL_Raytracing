#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "Primitives.h"
#include "Lights.h"

struct Scene{
    std::vector<glm::vec3> _vertices;
    std::vector<std::pair<Triangle, int>> _triangles;
    std::vector<std::pair<Sphere, int>> _spheres;
    std::vector<std::pair<Plane, int>> _planes;
    std::vector<PointLight> _pointLights;
    std::vector<DirectionalLight> _directionalLights;
    std::vector<Material> _materials;

    void clear() {
        _vertices.clear();
        _triangles.clear();
        _spheres.clear();
        _planes.clear();
        _pointLights.clear();
        _directionalLights.clear();
        _materials.clear();
    }
};