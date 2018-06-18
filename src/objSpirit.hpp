#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <array>
#include <string>

namespace objSpirit{

  enum class Warnings : unsigned int {
    Unused_Material =    1 << 0,
    Unknown_Expression = 1 << 1,
    All = ~(unsigned int)(0),
    No = 0
  };

  constexpr inline int operator&(const Warnings x, const Warnings y) {
    return static_cast<int>(x)& static_cast<int>(y);
  }

  constexpr inline Warnings operator|(const Warnings x, const Warnings y) {
    return static_cast<Warnings>(static_cast<int>(x) | static_cast<int>(y));
  }

  constexpr inline bool operator==(const Warnings x, const Warnings y){
    return static_cast<int>(x) == static_cast<int>(y);
  }

  class objLoader{
    using faceDesc = std::array<std::array<int64_t, 3>, 3>;

    struct MaterialDesc{
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
        { }
    };

    struct Material {
      MaterialDesc        desc;
      std::vector<size_t> faces;
      bool                loaded; //Has been loaded from MTL lib?

      Material() : loaded(false) {}
    };

    struct Group {
      std::string         name;
      std::vector<size_t> faces;
    };

    struct Face {
      faceDesc face;
      size_t   material;
      size_t   group;
    };

  public:

    struct Data{
      std::vector<std::array<float, 3>> _v;
      std::vector<std::array<float, 2>> _vt;
      std::vector<std::array<float, 3>> _vn;
      std::vector<Face>     _faces;
      std::vector<Material> _materials;
      std::vector<Group>    _groups;
      std::string _mtllib;

      void clearData() {
        _v.clear(); _v.shrink_to_fit();
        _vt.clear(); _vt.shrink_to_fit();
        _vn.clear(); _vn.shrink_to_fit();
        _faces.clear(); _faces.shrink_to_fit();
        _materials.clear(); _materials.shrink_to_fit();
        _groups.clear(); _groups.shrink_to_fit();
        _mtllib.clear();
      }
    };

    objLoader(){}
    objLoader(objLoader&) = delete;
    objLoader& operator=(objLoader& rhs) = delete;

    void loadObjFile(const std::string& filepath, const std::string& filename, const Warnings flags = Warnings::No);

    Data&& getData(){
      return std::move(_data);
    }

  private:
    std::string _parsedLine;
    Data        _data;
    std::map<std::string, size_t> _groupCache;
    std::map<std::string, size_t> _materialCache;

    bool parse_face(faceDesc&);
    bool parse_quad(faceDesc&, faceDesc&);

    size_t addFace(faceDesc&, size_t, size_t);
    size_t addGroup(const std::string&);
    size_t addMaterial(const std::string&);

    void convertFaceIndices(faceDesc&);

    void checkForErrors();

    void checkForWarnings(const Warnings flags);

    void clearData();

    bool loadMTL(const std::string&, const Warnings flags = Warnings::No);
  };

}