#include "SceneLoader.h"

SceneLoader::SceneLoader(const std::shared_ptr<OpenGLRaytracer> &raytracer, const std::shared_ptr<ShaderManager> &shManager) :
    m_numberOfObjInShader(0),
    m_numberOfMaterialsInShader(0),
    m_numberOfLightsInShader(0),
    m_shManager(shManager)
{
    initialize(raytracer);
}

SceneLoader::~SceneLoader()
    {
        delete[] m_oIndices;
        delete[] m_mIndices;
        delete[] m_lIndices;
        delete[] m_oOffsets;
        delete[] m_mOffsets;
        delete[] m_lOffsets;
    }

void SceneLoader::initialize(const std::shared_ptr<OpenGLRaytracer> &raytracer)
{
    m_materialElements = 4, m_objectElements = 5, m_lightElements = 3;
    const GLchar* oNames[] = {"objects[0].p.A", "objects[0].p.B", "objects[0].p.C", "objects[0].type", "objects[0].material_index"};
    const GLchar* mNames[] = {"materials[0].diffuse", "materials[0].specularity", "materials[0].emission", "materials[0].shininess"};
    const GLchar* lNames[] = {"lights[0].pos_dir", "lights[0].color", "lights[0].attenuation"};

    m_oIndices = new int[m_objectElements+1];
    m_mIndices = new int[m_materialElements+1];
    m_lIndices = new int[m_lightElements];
    m_oOffsets = new int[m_objectElements+1];
    m_mOffsets = new int[m_materialElements+1];
    m_lOffsets = new int[m_lightElements];

    const GLenum props[] = {GL_BUFFER_DATA_SIZE};
    const GLenum props2[] = {GL_OFFSET};

    auto compShaderName = raytracer->getCompShaderProgName();
    auto compShaderID = m_shManager->getShaderProgramID(compShaderName);

    m_shManager->getBufferVariableIndices(compShaderName, m_objectElements, oNames, m_oIndices);
    m_shManager->getBufferVariableIndices(compShaderName, m_materialElements, mNames, m_mIndices);
    m_shManager->getBufferVariableIndices(compShaderName, m_lightElements, lNames, m_lIndices);

    m_oBlockIndex = glGetProgramResourceIndex(compShaderID, GL_SHADER_STORAGE_BLOCK, "PrimitiveBuffer");
    m_mBlockIndex = glGetProgramResourceIndex(compShaderID, GL_SHADER_STORAGE_BLOCK, "MaterialBuffer");
    m_lBlockIndex = glGetProgramResourceIndex(compShaderID, GL_SHADER_STORAGE_BLOCK, "LightBuffer");


    for(int i = 0; i < m_materialElements; ++i){
        glGetProgramResourceiv(compShaderID, GL_BUFFER_VARIABLE, m_mIndices[i], 1, props2, 1, NULL, &m_mOffsets[i]);
    }

    for(int i = 0; i < m_objectElements; ++i){
        glGetProgramResourceiv(compShaderID, GL_BUFFER_VARIABLE, m_oIndices[i], 1, props2, 1, NULL, &m_oOffsets[i]);
    }

    for(int i = 0; i < m_lightElements; ++i){
        glGetProgramResourceiv(compShaderID, GL_BUFFER_VARIABLE, m_lIndices[i], 1, props2, 1, NULL, &m_lOffsets[i]);
    }

    glGetProgramResourceiv(compShaderID, GL_SHADER_STORAGE_BLOCK, m_oBlockIndex, 1, props, 1, NULL, &m_oBlockSize);
    glGetProgramResourceiv(compShaderID, GL_SHADER_STORAGE_BLOCK, m_mBlockIndex, 1, props, 1, NULL, &m_mBlockSize);
    glGetProgramResourceiv(compShaderID, GL_SHADER_STORAGE_BLOCK, m_lBlockIndex, 1, props, 1, NULL, &m_lBlockSize);

    m_oAlignOffset = (m_oBlockSize%16 == 0 ? m_oBlockSize : m_oBlockSize-(m_oBlockSize%16)+16);
    m_mAlignOffset = (m_mBlockSize%16 == 0 ? m_mBlockSize : m_mBlockSize-(m_mBlockSize%16)+16);
    m_lAlignOffset = (m_lBlockSize%16 == 0 ? m_lBlockSize : m_lBlockSize-(m_lBlockSize%16)+16);

    //Keep in mind to change the array size in the OpenGLRaytracer class when you change the amount of generated buffers
    glGenBuffers(3, raytracer->getStorageBufferIDs());
}

void SceneLoader::loadScene(const std::shared_ptr<Scene> &scene, const std::shared_ptr<OpenGLRaytracer> &raytracer)
{
    ////////////////////////////////////////////////////// LOADING OBJECTS //////////////////////////////////////////////////////
    void* p = malloc(m_numberOfObjInShader*m_oAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, raytracer->getStorageBufferIDs()[0]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numberOfObjInShader*m_oAlignOffset, (void *)(p));

    if(m_numberOfObjInShader*m_oAlignOffset == 0){
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numberOfObjInShader+scene->numberOfObjects())*m_oAlignOffset, NULL, GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numberOfObjInShader+scene->numberOfObjects())*m_oAlignOffset, p, GL_DYNAMIC_DRAW);
        free(p);
    }

    if(scene->numberOfObjects() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        int type = 3;
        for(const auto &tr : scene->triangles){
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[0], &(tr.first.A) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[1], &(tr.first.B) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[2], &(tr.first.C) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[3], &type , sizeof(int));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[4], &(tr.second) , sizeof(int));
            m_numberOfObjInShader++;
        }

        type = 1;
        for(const auto &sp : scene->spheres){
            const auto sphere = glm::vec4(sp.first.center, sp.first.radius);

            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[0], &sphere , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[3], &type , sizeof(int));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[4], &(sp.second) , sizeof(int));
            m_numberOfObjInShader++;
        }

        type = 2;
        for(const auto &pl : scene->planes){
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[0], &(pl.first.pos) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[1], &(pl.first.normal), sizeof(glm::vec4));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[3], &type , sizeof(int));
            memcpy(ptr + m_numberOfObjInShader*m_oAlignOffset + m_oOffsets[4], &(pl.second) , sizeof(int));
            m_numberOfObjInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_shManager->getShaderProgramID(raytracer->getCompShaderProgName()), m_oBlockIndex, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ////////////////////////////////////////////////////// LOADING MATERIALS //////////////////////////////////////////////////////

    p = malloc(m_numberOfMaterialsInShader*m_mAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, raytracer->getStorageBufferIDs()[1]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numberOfMaterialsInShader*m_mAlignOffset, p);
    if(m_numberOfMaterialsInShader*m_mAlignOffset == 0){
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numberOfMaterialsInShader + scene->materials.size()) * m_mAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numberOfMaterialsInShader + scene->materials.size()) * m_mAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if(scene->materials.size() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for(const auto &mt : scene->materials){
            memcpy(ptr + m_numberOfMaterialsInShader*m_mAlignOffset + m_mOffsets[0], &(mt.diffuse) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfMaterialsInShader*m_mAlignOffset + m_mOffsets[1], &(mt.specularity) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfMaterialsInShader*m_mAlignOffset + m_mOffsets[2], &(mt.emission) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfMaterialsInShader*m_mAlignOffset + m_mOffsets[3], &(mt.shininess) , sizeof(float));
            m_numberOfMaterialsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_shManager->getShaderProgramID(raytracer->getCompShaderProgName()), m_mBlockIndex, 1);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }


    ////////////////////////////////////////////////////// LOADING LIGHTS //////////////////////////////////////////////////////
    p = malloc(m_numberOfLightsInShader*m_lAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, raytracer->getStorageBufferIDs()[2]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_numberOfLightsInShader*m_lAlignOffset, p);
    if (m_numberOfLightsInShader*m_lAlignOffset == 0){
        glBufferData(GL_SHADER_STORAGE_BUFFER, scene->numberOfLights() * m_lAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (m_numberOfLightsInShader + scene->numberOfLights()) * m_lAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (scene->numberOfLights() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for(const auto &pl : scene->pointLights){
            memcpy(ptr + m_numberOfLightsInShader*m_lAlignOffset + m_lOffsets[0], &(pl.position) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfLightsInShader*m_lAlignOffset + m_lOffsets[1], &(pl.color) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfLightsInShader*m_lAlignOffset + m_lOffsets[2], &(pl.attenuation) , sizeof(glm::vec4));
            m_numberOfLightsInShader++;
        }

        for(const auto &dl : scene->directionalLights){
            memcpy(ptr + m_numberOfLightsInShader*m_lAlignOffset + m_lOffsets[0], &(dl.direction) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfLightsInShader*m_lAlignOffset + m_lOffsets[1], &(dl.color) , sizeof(glm::vec4));
            memcpy(ptr + m_numberOfLightsInShader*m_lAlignOffset + m_lOffsets[2], &(dl.attenuation) , sizeof(glm::vec4));
            m_numberOfLightsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_shManager->getShaderProgramID(raytracer->getCompShaderProgName()), m_lBlockIndex, 2);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    m_shManager->loadUniform_(raytracer->getCompShaderProgName(), std::string("numObj"), m_numberOfObjInShader);
    m_shManager->loadUniform_(raytracer->getCompShaderProgName(), std::string("numLights"), m_numberOfLightsInShader);
}