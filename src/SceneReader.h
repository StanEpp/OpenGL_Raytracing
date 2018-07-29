#pragma once

#include <string>
#include "Scene.h"
#include "objSpirit.hpp"

class SceneReader
{
public:
    void readScene(const std::string &filepath, Scene& scene)
    {
        objSpirit::objLoader reader;
        reader.loadObjFile(filepath, objSpirit::Warnings::All);
        auto data = reader.getData();

        scene.vertices.reserve(data.v.size());
        for (const auto &v : data.v) {
            scene.vertices.emplace_back(v[0], v[1], v[2]);
        }

        scene.triangles.reserve(data.faces.size());
        for (const auto &t : data.faces) {
            Triangle tr;
            const auto &v1 = data.v[t.face[0][0]];
            const auto &v2 = data.v[t.face[1][0]];
            const auto &v3 = data.v[t.face[2][0]];
            tr.A = glm::vec4(v1[0], v1[1], v1[2], 1.f);
            tr.B = glm::vec4(v2[0], v2[1], v2[2], 1.f);
            tr.C = glm::vec4(v3[0], v3[1], v3[2], 1.f);
            scene.triangles.push_back(std::make_pair(tr, t.material));;
        }

        scene.spheres.reserve(data.spheres.size());
        for (const auto &s : data.spheres) {
            Sphere sp;
            auto c = data.v[s.center];
            sp.center = glm::vec3(c[0], c[1], c[2]);
            sp.radius = s.radius;
            scene.spheres.push_back(std::make_pair(sp, s.material));
        }

        scene.planes.reserve(data.planes.size());
        for (const auto &p : data.planes) {
            Plane plane;
            const auto &o = data.v[p.origin];
            const auto &n = data.vn[p.normal];
            plane.pos = glm::vec4(o[0], o[1], o[2], 1.f);
            plane.normal = glm::vec4(n[0], n[1], n[2], 0.f);
            scene.planes.push_back(std::make_pair(plane, p.material));
        }

        scene.pointLights.reserve(data.pointLights.size());
        for (const auto &p : data.pointLights) {
            const auto &pos = p.center;
            const auto &col = p.color;
            const auto &att = p.attenuation;

            PointLight pl;
            pl.position = glm::vec4(pos[0], pos[1], pos[2], 1.f);
            pl.color = glm::vec4(col[0], col[1], col[2], 0.f);
            pl.attenuation = glm::vec4(att[0], att[1], att[2], 0.f);
            scene.pointLights.push_back(pl);
        }

        scene.directionalLights.reserve(data.directionalLights.size());
        for (const auto &d : data.directionalLights) {
            const auto &dir = d.direction;
            const auto &col = d.color;
            const auto &att = d.attenuation;

            DirectionalLight dl;
            dl.direction = glm::vec4(dir[0], dir[1], dir[2], 1.f);
            dl.color = glm::vec4(col[0], col[1], col[2], 0.f);
            dl.attenuation = glm::vec4(att[0], att[1], att[2], 0.f);
            scene.directionalLights.push_back(dl);
        }

        scene.materials.reserve(data.materials.size());
        for (const auto &m : data.materials) {
            const auto &diff = m.desc.diff;
            const auto &spec = m.desc.spec;
            const auto &em = m.desc.emission;

            Material mat;
            mat.diffuse = glm::vec4(diff[0], diff[1], diff[2], 0.f);
            mat.specularity = glm::vec4(spec[0], spec[1], spec[2], 0.f);
            mat.emission = glm::vec4(em[0], em[1], em[2], 0.f);
            mat.shininess = m.desc.shiny;
            scene.materials.push_back(mat);
        }
    }
};