#include "objSpirit.hpp"
#include <boost/spirit/home/x3.hpp>

namespace x3 = boost::spirit::x3;

using x3::float_;
using x3::int_;
using x3::_attr;
using x3::eoi;
using x3::parse;
using x3::phrase_parse;
using x3::lexeme;
using x3::ascii::space;
using x3::ascii::char_;

auto& vecToArr2(const std::vector<float>& vec2, std::array<float, 2>& arr2)
{
    arr2[0] = vec2[0];
    arr2[1] = vec2[1];
    return arr2;
}

auto& vecToArr3(const std::vector<float>& vec3, std::array<float, 3>& arr3)
{
    arr3[0] = vec3[0];
    arr3[1] = vec3[1];
    arr3[2] = vec3[2];
    return arr3;
}

auto& trimString(std::string &str)
{
    auto predicate = [](auto& c){
        if(c != ' ' && c != '\"') return true;
        return false;
    };
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), predicate));
    str.erase(std::find_if(str.rbegin(), str.rend(), predicate).base(), str.end());
    return str;
}

void objLoader::loadObjFile(const std::string& filepath, const std::string& filename, const Warnings flags)
{
    std::ifstream file(filepath + "\\" + filename, std::ios::in);

    if (!file.is_open()){
        std::cerr << "ERROR: Could not open file \"" << filepath << "\\" << filename << "\"" << std::endl;
        return;
    }

    clearData();

    auto itBegin = _parsedLine.begin();
    auto itEnd = _parsedLine.end();
    uint32_t linecounter = 0;
    std::array<float, 3> arr3;
    std::array<float, 2> arr2;
    objLoader::faceDesc face, face2;
    std::vector<float> vec;
    vec.reserve(3);
    std::string mat, tempStr;
    size_t currMatIdx = 0, currGroupIdx = 0;
    bool usedMaterial = false, usedGroup = false;

    auto checkMaterialAndGroup = [&]() {
        if (!usedMaterial) {
            currMatIdx = addMaterial("");
            usedMaterial = true;
        }
        if (!usedGroup) {
            currGroupIdx = addGroup("");
            usedGroup = true;
        }
    };

    // Define all grammar rules and actions
    auto const r_3float = float_ >> float_ >> float_;
    auto const r_2float = float_ >> float_;
    auto const r_readString =  lexeme[*(char_)];
    ////////////////////////////////////////////////////

    while (!file.eof()){
        getline(file, _parsedLine);
        linecounter++;
        itBegin = _parsedLine.begin();
        itEnd = _parsedLine.end();
        tempStr.clear();
        vec.clear();

        if (phrase_parse(itBegin, itEnd, "v " >> r_3float, space, vec)){
            _data._v.push_back(vecToArr3(vec, arr3));
        }
        else if (phrase_parse(itBegin, itEnd, "vt " >> r_2float, space, vec)) {
            _data._vt.push_back(vecToArr2(vec, arr2));
        }
        else if (phrase_parse(itBegin, itEnd, "vn " >> r_3float, space, vec)) {
            _data._vn.push_back(vecToArr3(vec, arr3));
        }
        else if (parse_face(face)) {
            checkMaterialAndGroup();
            addFace(face, currMatIdx, currGroupIdx);
        }
        else if (parse_quad(face, face2)) {
           checkMaterialAndGroup();
           addFace(face, currMatIdx, currGroupIdx);
           addFace(face2, currMatIdx, currGroupIdx);
        }
        else if (phrase_parse(itBegin, itEnd, "g " >> r_readString, space, tempStr)) {
            currGroupIdx = addGroup(trimString(tempStr));
            usedGroup = true;
        }
        else if (phrase_parse(itBegin, itEnd, "o " >> r_readString, space, tempStr)) {
            currGroupIdx = addGroup(trimString(tempStr));
            usedGroup = true;
        }
        else if (phrase_parse(itBegin, itEnd, "usemtl " >> r_readString, space, tempStr)) {
            currMatIdx = addMaterial(trimString(tempStr));
            usedMaterial = true;
        }
        else if (phrase_parse(itBegin, itEnd, "mtllib " >> r_readString, space, tempStr)) {
            if (!loadMTL(filepath + "\\" + trimString(tempStr), flags)) {
                std::cerr << "ERROR: Error while loading Material library \""<< _data._mtllib << "\"!" << std::endl;
                return;
            }
        }
        else if (flags & Warnings::Unknown_Expression || flags == Warnings::All){
            if (itBegin != itEnd && !phrase_parse(itBegin, _parsedLine.begin() + 1, "#", space))
                std::cerr << "WARNING: Unknown Expression \"" << _parsedLine << "\" in \"" << filepath << "\" at line " << linecounter << "!" << std::endl;
        }
    }

    file.close();

    checkForErrors();
    checkForWarnings(flags);

    _materialCache.clear();
    _groupCache.clear();
}

bool objLoader::loadMTL(const std::string& filepath, const Warnings flags) {
    std::ifstream file(filepath, std::ios::in);

    if (!file.is_open()) {
        std::cerr << "ERROR: Couldn't open mtllib \"" << filepath + "\\" + _data._mtllib << "\"" << std::endl;
        return false;
    }

    std::vector<float> vec3;
    vec3.reserve(3);
    std::string tempStr;
    auto itBegin = _parsedLine.begin();
    auto itEnd = _parsedLine.end();
    uint32_t linecounter = 0;
    Material* currMaterial = nullptr;
    float fValue = 0.f;
    int iValue = 0;

    // Define all grammar rules and actions
    auto assignFloat = [&fValue](auto& ctx) {fValue = _attr(ctx); };
    auto assignInt = [&iValue](auto& ctx) {iValue = _attr(ctx); };
    auto const r_3float = float_ >> float_ >> float_;
    auto const r_floatToValue = float_[assignFloat];
    auto const r_intToValue = float_[assignInt];
    auto const r_readString = lexeme[*(char_)];
    ////////////////////////////////////////////////////

    while (!file.eof()) {
        getline(file, _parsedLine);
        linecounter++;
        itBegin = _parsedLine.begin();
        itEnd = _parsedLine.end();
        tempStr.clear();
        vec3.clear();

        if (phrase_parse(itBegin, itEnd, "Ka " >> r_3float, space, vec3)) {
            vecToArr3(vec3, currMaterial->desc.amb);
        }
        else if (phrase_parse(itBegin, itEnd, "Kd " >> r_3float, space, vec3)) {
            vecToArr3(vec3, currMaterial->desc.diff);
        }
        else if (phrase_parse(itBegin, itEnd, "Ks " >> r_3float, space, vec3)) {
            vecToArr3(vec3, currMaterial->desc.spec);
        }
        else if (phrase_parse(itBegin, itEnd, "Ke " >> r_3float, space, vec3)) {
            vecToArr3(vec3, currMaterial->desc.emission);
        }
        else if (phrase_parse(itBegin, itEnd, "Ns" >> r_floatToValue, space)) {
            currMaterial->desc.shiny = fValue;
        }
        else if (phrase_parse(itBegin, itEnd, "d" >> r_floatToValue, space)) {
            currMaterial->desc.trans = fValue;
        }
        else if (phrase_parse(itBegin, itEnd, "r" >> r_floatToValue, space)) {
            currMaterial->desc.reflect = fValue;
        }
        else if (phrase_parse(itBegin, itEnd, "sharpness" >> r_floatToValue, space)) {
            currMaterial->desc.glossy = fValue;
        }
        else if (phrase_parse(itBegin, itEnd, "Ni" >> r_floatToValue, space)) {
            currMaterial->desc.refract_index = fValue;
        }
        else if (phrase_parse(itBegin, itEnd, "illum" >> r_intToValue, space)) {
            currMaterial->desc.illum = iValue;
        }
        else if (phrase_parse(itBegin, itEnd, "map_Kd " >> r_readString, space, tempStr)) {
            currMaterial->desc.map_Kd = trimString(tempStr);
        }
        else if (phrase_parse(itBegin, itEnd, "map_Ka " >> r_readString, space, tempStr)) {
            currMaterial->desc.map_Ka = trimString(tempStr);
        }
        else if (phrase_parse(itBegin, itEnd, "map_Ks " >> r_readString, space, tempStr)) {
            currMaterial->desc.map_Ks = trimString(tempStr);
        }
        else if (phrase_parse(itBegin, itEnd, ("map_bump " >> r_readString) | ("map_Bump " >> r_readString), space, tempStr)) {
            currMaterial->desc.map_normal = trimString(tempStr);
        }
        else if (phrase_parse(itBegin, itEnd, "map_d " >> r_readString, space, tempStr)) {
            currMaterial->desc.map_d = trimString(tempStr);
        }
        else if (phrase_parse(itBegin, itEnd, "newmtl " >> r_readString, space, tempStr)) {
            auto idx = addMaterial(trimString(tempStr));
            currMaterial = &(_data._materials[idx]);
            currMaterial->loaded = true;
        }
        else if (flags &  Warnings::Unknown_Expression || flags == Warnings::All) {
            if (itBegin != itEnd && !phrase_parse(itBegin, _parsedLine.begin() + 1, "#", space))
                std::cerr << "WARNING: Unknown Expression \"" << _parsedLine << "\" in \"" << _data._mtllib << "\" at line " << linecounter << "!" << std::endl;
        }
    }
    file.close();

    _data._mtllib = filepath;

    return true;
}

size_t objLoader::addFace(objLoader::faceDesc& face, size_t matIdx, size_t groupIdx) {
    Face newFace;
    newFace.face = face;
    newFace.material = matIdx;
    newFace.group = groupIdx;

    _data._faces.push_back(newFace);
    auto faceIdx = _data._faces.size() - 1;
    _data._materials[matIdx].faces.push_back(faceIdx);
    _data._groups[groupIdx].faces.push_back(faceIdx);

    return faceIdx;
}

size_t objLoader::addGroup(const std::string& name) {
    auto it = _groupCache.find(name);
    if (it != _groupCache.end()) {
        return it->second;
    } else {
        Group newGroup;
        newGroup.name = name;
        _data._groups.push_back(newGroup);
        auto idx = _data._groups.size() - 1;
        _groupCache[name] = idx;
        return idx;
    }
}

size_t objLoader::addMaterial(const std::string& name) {
    auto it = _materialCache.find(name);
    //Check if material already exists. If not, create a new one.
    //If yes, use the already created material.
    if (it == _materialCache.end()) {
        Material newMaterial;
        newMaterial.desc.name = name;
        _data._materials.push_back(newMaterial);
        auto idx = _data._materials.size() - 1;
        _materialCache[name] = idx;
        return idx;
    } else {
        return it->second;
    }
}

bool objLoader::parse_face(objLoader::faceDesc& face) {
    face = {{ {{ 0, 0, 0 }}, {{ 0, 0, 0 }}, {{ 0, 0, 0 }} }};
    auto f = [&face](int idx1, int idx2){ return [=, &face](auto& ctx){face[idx1][idx2] = _attr(ctx);};};
    auto success = phrase_parse(_parsedLine.begin(), _parsedLine.end(),
    (
        "f " >> int_[f(0,0)] >> '/'  >> int_[f(0,1)]  >> '/' >> int_[f(0,2)] >>
                int_[f(1,0)] >> '/'  >> int_[f(1,1)]  >> '/' >> int_[f(1,2)] >>
                int_[f(2,0)] >> '/'  >> int_[f(2,1)]  >> '/' >> int_[f(2,2)] >> eoi | // v/vt/vn v/vt/vn v/vt/vn
        "f " >> int_[f(0,0)] >> "//" >> int_[f(0,2)]  >>
                int_[f(1,0)] >> "//" >> int_[f(1,2)]  >>
                int_[f(2,0)] >> "//" >> int_[f(2,2)]  >> eoi | // v//vn v//vn v//vn
        "f " >> int_[f(0,0)] >> '/'  >> int_[f(0,1)]  >>
                int_[f(1,0)] >> '/'  >> int_[f(1,1)]  >>
                int_[f(2,0)] >> '/'  >> int_[f(2,1)]  >> eoi | // v/vt v/vt v/vt
        "f " >> int_[f(0,0)] >>         int_[f(1,0)]  >>        int_[f(2,0)] >> eoi // v v v
    ),
    space);

    if (success) convertFaceIndices(face);

    return success;
}

bool objLoader::parse_quad(objLoader::faceDesc& face1, objLoader::faceDesc& face2) {
    face1 = {{ {{ 0, 0, 0 }}, {{ 0, 0, 0 }}, {{ 0, 0, 0 }} }};
    face2 = {{ {{ 0, 0, 0 }}, {{ 0, 0, 0 }}, {{ 0, 0, 0 }} }};

    std::array<std::array<int64_t, 3>, 4> quad = {{ {{ 0, 0, 0 }}, {{ 0, 0, 0 }} , {{ 0, 0, 0 }} , {{ 0, 0, 0 }} }};

    auto f = [&quad](int idx1, int idx2){ return [=, &quad](auto& ctx){quad[idx1][idx2] = _attr(ctx);};};

    auto success = phrase_parse(_parsedLine.begin(), _parsedLine.end(),
    (
        "f " >> int_[f(0,0)] >> '/'  >> int_[f(0,1)] >> '/' >> int_[f(0,2)] >>
                int_[f(1,0)] >> '/'  >> int_[f(1,1)] >> '/' >> int_[f(1,2)] >>
                int_[f(2,0)] >> '/'  >> int_[f(2,1)] >> '/' >> int_[f(2,2)] >>
                int_[f(3,0)] >> '/'  >> int_[f(3,1)] >> '/' >> int_[f(3,2)] >> eoi | // v/vt/vn v/vt/vn v/vt/vn
        "f " >> int_[f(0,0)] >> "//" >> int_[f(0,2)] >>
                 int_[f(1,0)] >> "//" >> int_[f(1,2)] >>
                int_[f(2,0)] >> "//" >> int_[f(2,2)] >>
                int_[f(3,0)] >> "//" >> int_[f(3,2)] >> eoi | // v//vn v//vn v//vn
        "f " >> int_[f(0,0)] >> "/"  >> int_[f(0,1)] >>
                int_[f(1,0)] >> '/'  >> int_[f(1,1)] >>
                int_[f(2,0)] >> '/'  >> int_[f(2,1)] >>
                int_[f(3,0)] >> '/'  >> int_[f(3,1)] >> eoi | // v/vt v/vt v/vt
        "f " >> int_[f(0,0)] >>         int_[f(1,0)] >> int_[f(2,0)] >> int_[f(3,0)] >> eoi // v v v
    ),
    space);

    if (success){
        face1[0] = quad[0];
        face1[1] = quad[1];
        face1[2] = quad[2];
        face2[0] = quad[2];
        face2[1] = quad[3];
        face2[2] = quad[0];

        convertFaceIndices(face1);
        convertFaceIndices(face2);
    }

    return success;
}

// Converts negative indices to positive indices and decrements positive values by -1.
// If index is a 0 (which is an invalid value and signals that attribute is not existent) it will be converted to -1.
void objLoader::convertFaceIndices(objLoader::faceDesc& face) {
    for (size_t i = 0; i < 3; i++) {
        if (face[i][0] < 0) {
            face[i][0] += _data._v.size();
        }
        else {
            face[i][0]--;
        }
    }

    for (size_t i = 0; i < 3; i++) {
        if (face[i][1] < 0) {
            face[i][1] += _data._vt.size();
        }
        else {
            face[i][1]--;
        }
    }

    for (size_t i = 0; i < 3; i++) {
        if (face[i][2] < 0) {
            face[i][2] += _data._vn.size();
        }
        else {
            face[i][2]--;
        }
    }
}

void objLoader::checkForErrors(){
    for (auto it : _data._materials){
        if (!it.loaded && !it.desc.name.empty()){
            std::cerr << "ERROR: Material \"" << it.desc.name << "\" is not found in \"" << _data._mtllib << "\"!" << std::endl;
        }
    }
}

void objLoader::checkForWarnings(const Warnings flags){
    if (flags &  Warnings::Unused_Material || flags == Warnings::All){
        for (auto it : _data._materials){
            if (it.faces.empty()){
                std::cerr << "Warning: Material \"" << it.desc.name << "\" is not used!" << std::endl;
            }
        }
    }
}

void objLoader::clearData(){
    _parsedLine.clear();
    _data.clearData();
    _groupCache.clear();
    _materialCache.clear();
}