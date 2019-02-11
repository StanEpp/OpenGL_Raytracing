#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <array>
#include <string>

namespace objSpirit{

enum class Warnings : unsigned int
{
    Unused_Material =    1 << 0,
    Unknown_Expression = 1 << 1,
    All = ~0u,
    No = 0u
};

constexpr inline int operator&(const Warnings x, const Warnings y)
{
    return static_cast<int>(x)& static_cast<int>(y);
}

constexpr inline Warnings operator|(const Warnings x, const Warnings y)
{
    return static_cast<Warnings>(static_cast<int>(x) | static_cast<int>(y));
}

constexpr inline bool operator==(const Warnings x, const Warnings y)
{
    return static_cast<int>(x) == static_cast<int>(y);
}

class objLoader
{
    using FaceDesc = std::array<std::array<int64_t, 3>, 3>;

    struct MaterialDesc
    {
        std::string name;
        std::array<float, 3> amb;
        std::array<float, 3> diff;
        std::array<float, 3> spec;
        std::array<float, 3> emission;
        float reflect;
        float refract;
        float trans;
        float shiny;
        float glossy;
        float refract_index;
        int illum;
        std::string map_Ka;
        std::string map_Kd;
        std::string map_Ks;
        std::string map_d;
        std::string map_normal;

        MaterialDesc() : name(), amb{0, 0, 0}, diff{0, 0, 0}, spec{0, 0, 0}, emission{0, 0, 0},
            reflect(0.f), refract(0.f), trans(0.f), shiny(0.f), glossy(0.f), refract_index(0.f), illum(0),
            map_Ka(), map_Kd(), map_Ks(), map_d(), map_normal()
        {}
    };

    struct Material
    {
        MaterialDesc        desc;
        std::vector<size_t> faces;
        std::vector<size_t> spheres;
        std::vector<size_t> planes;
        bool                loaded; //Has been loaded from MTL lib?

        Material() : loaded(false) {}
    };

    struct Group
    {
        std::string         name;
        std::vector<size_t> faces;
        std::vector<size_t> spheres;
        std::vector<size_t> planes;
    };

    struct Face
    {
        FaceDesc face;
        size_t material;
        size_t group;
    };

    struct Sphere
    {
        int64_t center;
        size_t material;
        float radius;
        size_t group;
    };

    struct Plane
    {
        int64_t origin;
        int64_t normal;
        size_t material;
        size_t group;
    };

    struct PointLight
    {
        std::array<float, 3> center;
        std::array<float, 3> color;
        std::array<float, 3> attenuation;
    };

    struct DirectionalLight
    {
        std::array<float, 3> direction;
        std::array<float, 3> color;
        std::array<float, 3> attenuation;
    };

public:

    struct Data
    {
        std::vector<std::array<float, 3>> v;
        std::vector<std::array<float, 2>> vt;
        std::vector<std::array<float, 3>> vn;
        std::vector<Face>             faces;
        std::vector<Sphere>           spheres;
        std::vector<Plane>            planes;
        std::vector<Material>         materials;
        std::vector<Group>            groups;
        std::vector<PointLight>       pointLights;
        std::vector<DirectionalLight> directionalLights;
        std::string mtllib;

        void clearData() {
            v.clear(); v.shrink_to_fit();
            vt.clear(); vt.shrink_to_fit();
            vn.clear(); vn.shrink_to_fit();
            faces.clear(); faces.shrink_to_fit();
            spheres.clear(); spheres.shrink_to_fit();
            planes.clear(); planes.shrink_to_fit();
            materials.clear(); materials.shrink_to_fit();
            groups.clear(); groups.shrink_to_fit();
            mtllib.clear();
        }
    };

    objLoader(){}
    objLoader(objLoader&) = delete;
    objLoader& operator=(objLoader& rhs) = delete;

    void loadObjFile(const std::string& filename, const Warnings flags = Warnings::No);

    Data&& getData() { return std::move(m_data); }

private:
    std::string m_parsedLine;
    Data m_data;
    std::map<std::string, size_t> m_groupCache;
    std::map<std::string, size_t> m_materialCache;

    bool parse_face(FaceDesc&);
    bool parse_quad(FaceDesc&, FaceDesc&);
    bool parse_sphere(Sphere&);
    bool parse_plane(Plane&);
    bool parse_pointLight(PointLight&);
    bool parse_dirLight(DirectionalLight&);

    size_t addFace(FaceDesc&, size_t, size_t);
    size_t addSphere(Sphere&, size_t, size_t);
    size_t addPlane(Plane&, size_t, size_t);
    size_t addPointLight(PointLight&);
    size_t addDirLight(DirectionalLight&);
    size_t addGroup(const std::string&);
    size_t addMaterial(const std::string&);

    void convertIndices(FaceDesc& face);
    void convertIndices(Sphere& sphere);
    void convertIndices(Plane& plane);

    void checkForErrors();

    void checkForWarnings(const Warnings flags);

    void clearData();

    bool loadMTL(const std::string&, const Warnings flags = Warnings::No);
};

}
