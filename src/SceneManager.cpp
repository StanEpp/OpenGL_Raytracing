#include "SceneManager.hpp"
#include "ShaderManager.hpp"

#include <objSpirit.hpp>


void SceneManager::readScene(const std::string &filepath)
{
    m_scene.clear();
    objSpirit::objLoader reader;
    reader.loadObjFile(filepath, objSpirit::Warnings::All);
    auto data = reader.getData();

    m_scene.vertices.reserve(data.v.size());
    for (const auto &v : data.v) {
        m_scene.vertices.emplace_back(v[0], v[1], v[2]);
    }

    m_scene.triangles.reserve(data.faces.size());
    for (const auto &t : data.faces) {
        Triangle tr;
        const auto &v1 = data.v[t.face[0][0]];
        const auto &v2 = data.v[t.face[1][0]];
        const auto &v3 = data.v[t.face[2][0]];
        tr.A = glm::vec4(v1[0], v1[1], v1[2], 1.f);
        tr.B = glm::vec4(v2[0], v2[1], v2[2], 1.f);
        tr.C = glm::vec4(v3[0], v3[1], v3[2], 1.f);
        m_scene.triangles.push_back(std::make_pair(tr, t.material));;
    }

    m_scene.spheres.reserve(data.spheres.size());
    for (const auto &s : data.spheres) {
        Sphere sp;
        auto c = data.v[s.center];
        sp.center = glm::vec3(c[0], c[1], c[2]);
        sp.radius = s.radius;
        m_scene.spheres.push_back(std::make_pair(sp, s.material));
    }

    m_scene.planes.reserve(data.planes.size());
    for (const auto &p : data.planes) {
        Plane plane;
        const auto &o = data.v[p.origin];
        const auto &n = data.vn[p.normal];
        plane.pos = glm::vec4(o[0], o[1], o[2], 1.f);
        plane.normal = glm::vec4(n[0], n[1], n[2], 0.f);
        m_scene.planes.push_back(std::make_pair(plane, p.material));
    }

    m_scene.pointLights.reserve(data.pointLights.size());
    for (const auto &p : data.pointLights) {
        const auto &pos = p.center;
        const auto &col = p.color;
        const auto &att = p.attenuation;

        // pl.attenuation.w = 1 encodes type pointlight
        PointLight pl;
        pl.position = glm::vec4(pos[0], pos[1], pos[2], 1.f);
        pl.color = glm::vec4(col[0], col[1], col[2], 0.f);
        pl.attenuation = glm::vec4(att[0], att[1], att[2], 1.f);
        m_scene.pointLights.push_back(pl);
    }

    m_scene.directionalLights.reserve(data.directionalLights.size());
    for (const auto &d : data.directionalLights) {
        const auto &dir = d.direction;
        const auto &col = d.color;
        const auto &att = d.attenuation;

        // dl.attenuation.w = 1 encodes type directionalLight
        DirectionalLight dl;
        dl.direction = glm::vec4(dir[0], dir[1], dir[2], 1.f);
        dl.color = glm::vec4(col[0], col[1], col[2], 0.f);
        dl.attenuation = glm::vec4(att[0], att[1], att[2], 2.f);
        m_scene.directionalLights.push_back(dl);
    }

    m_scene.materials.reserve(data.materials.size());
    for (const auto &m : data.materials) {
        const auto &diff = m.desc.diff;
        const auto &spec = m.desc.spec;
        const auto &em = m.desc.emission;

        Material mat;
        mat.diffuse = glm::vec4(diff[0], diff[1], diff[2], 0.f);
        mat.specularity = glm::vec4(spec[0], spec[1], spec[2], 0.f);
        mat.emission = glm::vec4(em[0], em[1], em[2], 0.f);
        mat.shininess = m.desc.shiny;
        m_scene.materials.push_back(mat);
    }
}

void SceneManager::initialize(GLuint computeShaderID)
{
    const GLchar* oNames[] = {"objects[0].p.A", "objects[0].p.B", "objects[0].p.C", "objects[0].type", "objects[0].material_index"};
    const GLchar* mNames[] = {"materials[0].diffuse", "materials[0].specularity", "materials[0].emission", "materials[0].shininess"};
    const GLchar* lNames[] = {"lights[0].pos_dir", "lights[0].color", "lights[0].attenuation"};

    auto oIndices = new int[NumAttributesObjects + 1];
    auto mIndices = new int[NumAttributesMaterial + 1];
    auto lIndices = new int[NumAttributesLights];
    m_oOffsets = new int[NumAttributesObjects + 1];
    m_mOffsets = new int[NumAttributesMaterial + 1];
    m_lOffsets = new int[NumAttributesLights];

    const GLenum props[] = {GL_BUFFER_DATA_SIZE};
    const GLenum props2[] = {GL_OFFSET};

    auto getIndices = [&](int length, const GLchar** const names, GLint* indices){
        for (int i = 0; i < length; ++i) {
            indices[i] = glGetProgramResourceIndex(computeShaderID, GL_BUFFER_VARIABLE, names[i]);
        }
    };

    getIndices(NumAttributesObjects, oNames, oIndices);
    getIndices(NumAttributesMaterial, mNames, mIndices);
    getIndices(NumAttributesLights, lNames, lIndices);

    m_oBlockIndex = glGetProgramResourceIndex(computeShaderID, GL_SHADER_STORAGE_BLOCK, "PrimitiveBuffer");
    m_mBlockIndex = glGetProgramResourceIndex(computeShaderID, GL_SHADER_STORAGE_BLOCK, "MaterialBuffer");
    m_lBlockIndex = glGetProgramResourceIndex(computeShaderID, GL_SHADER_STORAGE_BLOCK, "LightBuffer");

    for (unsigned int i = 0; i < NumAttributesMaterial; ++i) {
        glGetProgramResourceiv(computeShaderID, GL_BUFFER_VARIABLE, mIndices[i], 1, props2, 1, NULL, &m_mOffsets[i]);
    }

    for (unsigned int i = 0; i < NumAttributesObjects; ++i) {
        glGetProgramResourceiv(computeShaderID, GL_BUFFER_VARIABLE, oIndices[i], 1, props2, 1, NULL, &m_oOffsets[i]);
    }

    for (unsigned int i = 0; i < NumAttributesLights; ++i) {
        glGetProgramResourceiv(computeShaderID, GL_BUFFER_VARIABLE, lIndices[i], 1, props2, 1, NULL, &m_lOffsets[i]);
    }

    int oBlockSize = 0;
    int mBlockSize = 0;
    int lBlockSize = 0;
    glGetProgramResourceiv(computeShaderID, GL_SHADER_STORAGE_BLOCK, m_oBlockIndex, 1, props, 1, NULL, &oBlockSize);
    glGetProgramResourceiv(computeShaderID, GL_SHADER_STORAGE_BLOCK, m_mBlockIndex, 1, props, 1, NULL, &mBlockSize);
    glGetProgramResourceiv(computeShaderID, GL_SHADER_STORAGE_BLOCK, m_lBlockIndex, 1, props, 1, NULL, &lBlockSize);

    m_oAlignOffset = (oBlockSize%16 == 0 ? oBlockSize : oBlockSize-(oBlockSize%16)+16);
    m_mAlignOffset = (mBlockSize%16 == 0 ? mBlockSize : mBlockSize-(mBlockSize%16)+16);
    m_lAlignOffset = (lBlockSize%16 == 0 ? lBlockSize : lBlockSize-(lBlockSize%16)+16);
}

void SceneManager::uploadScenes(const std::vector<std::string> &filepaths, ShaderManager& shManager, GLuint computeShaderID, GLuint* computeShaderstorageBufferIDs)
{
    initialize(computeShaderID);

    for (const auto &filepath : filepaths) {
        std::cout << "Start reading scene: " << filepath << '\n';
        readScene(filepath);
        std::cout << "Completed reading scene: " << filepath << '\n';
        std::cout << "Start uploading scene: " << filepath << '\n';
        uploadScene(shManager, computeShaderID, computeShaderstorageBufferIDs);
        std::cout << "Completed uploading scene: " << filepath << "\n\n";
    }
}

void SceneManager::uploadScene(ShaderManager& shManager, GLuint computeShaderID, GLuint* computeShaderstorageBufferIDs)
{
    ////////////////////////////////////////////////////// LOADING OBJECTS //////////////////////////////////////////////////////
    void* p = malloc(m_numObjInShader * m_oAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, computeShaderstorageBufferIDs[0]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numObjInShader * m_oAlignOffset, (void *)(p));

    if (m_numObjInShader * m_oAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numObjInShader + m_scene.numObjects()) * m_oAlignOffset, NULL, GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numObjInShader + m_scene.numObjects()) * m_oAlignOffset, p, GL_DYNAMIC_DRAW);
        free(p);
    }

    if (m_scene.numObjects() != 0) {
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        int type = 3;
        for (const auto &tr : m_scene.triangles) {
            const auto matIdx = tr.second + m_numMaterialsInShader;

            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[0], &(tr.first.A) , sizeof(glm::vec4));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[1], &(tr.first.B) , sizeof(glm::vec4));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[2], &(tr.first.C) , sizeof(glm::vec4));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[3], &type , sizeof(int));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[4], &(matIdx) , sizeof(int));
            m_numObjInShader++;
        }

        type = 1;
        for (const auto &sp : m_scene.spheres) {
            const auto sphere = glm::vec4(sp.first.center, sp.first.radius);
            const auto matIdx = sp.second + m_numMaterialsInShader;

            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[0], &sphere , sizeof(glm::vec4));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[3], &type , sizeof(int));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[4], &(matIdx) , sizeof(int));
            m_numObjInShader++;
        }

        type = 2;
        for (const auto &pl : m_scene.planes) {
            const auto matIdx = pl.second + m_numMaterialsInShader;

            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[0], &(pl.first.pos) , sizeof(glm::vec4));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[1], &(pl.first.normal), sizeof(glm::vec4));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[3], &type , sizeof(int));
            memcpy(ptr + m_numObjInShader * m_oAlignOffset + m_oOffsets[4], &(matIdx) , sizeof(int));
            m_numObjInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(computeShaderID, m_oBlockIndex, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ////////////////////////////////////////////////////// LOADING MATERIALS //////////////////////////////////////////////////////
    p = malloc(m_numMaterialsInShader * m_mAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, computeShaderstorageBufferIDs[1]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numMaterialsInShader * m_mAlignOffset, p);
    if (m_numMaterialsInShader * m_mAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + m_scene.materials.size()) * m_mAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numMaterialsInShader + m_scene.materials.size()) * m_mAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (!m_scene.materials.empty()) {
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &mt : m_scene.materials) {
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[0], &(mt.diffuse) , sizeof(glm::vec4));
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[1], &(mt.specularity) , sizeof(glm::vec4));
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[2], &(mt.emission) , sizeof(glm::vec4));
            memcpy(ptr + m_numMaterialsInShader * m_mAlignOffset + m_mOffsets[3], &(mt.shininess) , sizeof(float));
            m_numMaterialsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(computeShaderID, m_mBlockIndex, 1);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ////////////////////////////////////////////////////// LOADING LIGHTS //////////////////////////////////////////////////////
    p = malloc(m_numLightsInShader * m_lAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, computeShaderstorageBufferIDs[2]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numLightsInShader * m_lAlignOffset, p);
    if (m_numLightsInShader * m_lAlignOffset == 0) {
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_scene.numLights() * m_lAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numLightsInShader + m_scene.numLights()) * m_lAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (m_scene.numLights() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for (const auto &pl : m_scene.pointLights) {
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[0], &(pl.position) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[1], &(pl.color) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[2], &(pl.attenuation) , sizeof(glm::vec4));
            m_numLightsInShader++;
        }

        for (const auto &dl : m_scene.directionalLights) {
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[0], &(dl.direction) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[1], &(dl.color) , sizeof(glm::vec4));
            memcpy(ptr + m_numLightsInShader * m_lAlignOffset + m_lOffsets[2], &(dl.attenuation) , sizeof(glm::vec4));
            m_numLightsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(computeShaderID, m_lBlockIndex, 2);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    shManager.loadUniform_(computeShaderID, std::string("numObj"), m_numObjInShader);
    shManager.loadUniform_(computeShaderID, std::string("numLights"), m_numLightsInShader);
}