#include "SceneLoader.h"

SceneLoader::SceneLoader(const std::shared_ptr<OpenGLRaytracer> &raytracer, const std::shared_ptr<ShaderManager> &shManager) :
    _numberOfObjInShader(0),
    _numberOfMaterialsInShader(0),
    _numberOfLightsInShader(0),
    m_shManager(shManager)
{
    initialize(raytracer);
}

void SceneLoader::initialize(const std::shared_ptr<OpenGLRaytracer> &raytracer)
{
    _materialElements = 4, _objectElements = 5, _lightElements = 3;
    const GLchar* oNames[] = {"objects[0].p.A", "objects[0].p.B", "objects[0].p.C", "objects[0].type", "objects[0].material_index"};
    const GLchar* mNames[] = {"materials[0].diffuse", "materials[0].specularity", "materials[0].emission", "materials[0].shininess"};
    const GLchar* lNames[] = {"lights[0].pos_dir", "lights[0].color", "lights[0].attenuation"};

    _oIndices = new int[_objectElements+1];
    _mIndices = new int[_materialElements+1];
    _lIndices = new int[_lightElements];
    _oOffsets = new int[_objectElements+1];
    _mOffsets = new int[_materialElements+1];
    _lOffsets = new int[_lightElements];

    const GLenum props[] = {GL_BUFFER_DATA_SIZE};
    const GLenum props2[] = {GL_OFFSET};

    auto compShaderName = raytracer->getCompShaderProgName();
    auto compShaderID = m_shManager->getShaderProgramID(compShaderName);

    m_shManager->getBufferVariableIndices(compShaderName, _objectElements, oNames, _oIndices);
    m_shManager->getBufferVariableIndices(compShaderName, _materialElements, mNames, _mIndices);
    m_shManager->getBufferVariableIndices(compShaderName, _lightElements, lNames, _lIndices);

    _oBlockIndex = glGetProgramResourceIndex(compShaderID, GL_SHADER_STORAGE_BLOCK, "PrimitiveBuffer");
    _mBlockIndex = glGetProgramResourceIndex(compShaderID, GL_SHADER_STORAGE_BLOCK, "MaterialBuffer");
    _lBlockIndex = glGetProgramResourceIndex(compShaderID, GL_SHADER_STORAGE_BLOCK, "LightBuffer");


    for(int i = 0; i < _materialElements; ++i){
        glGetProgramResourceiv(compShaderID, GL_BUFFER_VARIABLE, _mIndices[i], 1, props2, 1, NULL, &_mOffsets[i]);
    }

    for(int i = 0; i < _objectElements; ++i){
        glGetProgramResourceiv(compShaderID, GL_BUFFER_VARIABLE, _oIndices[i], 1, props2, 1, NULL, &_oOffsets[i]);
    }

    for(int i = 0; i < _lightElements; ++i){
        glGetProgramResourceiv(compShaderID, GL_BUFFER_VARIABLE, _lIndices[i], 1, props2, 1, NULL, &_lOffsets[i]);
    }

    glGetProgramResourceiv(compShaderID, GL_SHADER_STORAGE_BLOCK, _oBlockIndex, 1, props, 1, NULL, &_oBlockSize);
    glGetProgramResourceiv(compShaderID, GL_SHADER_STORAGE_BLOCK, _mBlockIndex, 1, props, 1, NULL, &_mBlockSize);
    glGetProgramResourceiv(compShaderID, GL_SHADER_STORAGE_BLOCK, _lBlockIndex, 1, props, 1, NULL, &_lBlockSize);

    _oAlignOffset = (_oBlockSize%16 == 0 ? _oBlockSize : _oBlockSize-(_oBlockSize%16)+16);
    _mAlignOffset = (_mBlockSize%16 == 0 ? _mBlockSize : _mBlockSize-(_mBlockSize%16)+16);
    _lAlignOffset = (_lBlockSize%16 == 0 ? _lBlockSize : _lBlockSize-(_lBlockSize%16)+16);

    //Keep in mind to change the array size in the OpenGLRaytracer class when you change the amount of generated buffers
    glGenBuffers(3, raytracer->getStorageBufferIDs());
}

void SceneLoader::loadScene(Scene& scene, OpenGLRaytracer& ogl)
{
    ////////////////////////////////////////////////////// LOADING OBJECTS //////////////////////////////////////////////////////
    void* p = malloc(_numberOfObjInShader*_oAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ogl.getStorageBufferIDs()[0]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _numberOfObjInShader*_oAlignOffset, (void *)(p));

    if(_numberOfObjInShader*_oAlignOffset == 0){
        glBufferData(GL_SHADER_STORAGE_BUFFER, (_numberOfObjInShader+scene.numberOfObjects())*_oAlignOffset, NULL, GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (_numberOfObjInShader+scene.numberOfObjects())*_oAlignOffset, p, GL_DYNAMIC_DRAW);
        free(p);
    }

    if(scene.numberOfObjects() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        int type = 3;
        for(const auto &tr : scene.triangles){
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[0], &(tr.first.A) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[1], &(tr.first.B) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[2], &(tr.first.C) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[3], &type , sizeof(int));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[4], &(tr.second) , sizeof(int));
            _numberOfObjInShader++;
        }

        type = 1;
        for(const auto &sp : scene.spheres){
            const auto sphere = glm::vec4(sp.first.center, sp.first.radius);

            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[0], &sphere , sizeof(glm::vec4));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[3], &type , sizeof(int));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[4], &(sp.second) , sizeof(int));
            _numberOfObjInShader++;
        }

        type = 2;
        for(const auto &pl : scene.planes){
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[0], &(pl.first.pos) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[1], &(pl.first.normal), sizeof(glm::vec4));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[3], &type , sizeof(int));
            memcpy(ptr + _numberOfObjInShader*_oAlignOffset + _oOffsets[4], &(pl.second) , sizeof(int));
            _numberOfObjInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_shManager->getShaderProgramID(ogl.getCompShaderProgName()), _oBlockIndex, 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    ////////////////////////////////////////////////////// LOADING MATERIALS //////////////////////////////////////////////////////

    p = malloc(_numberOfMaterialsInShader*_mAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ogl.getStorageBufferIDs()[1]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _numberOfMaterialsInShader*_mAlignOffset, p);
    if(_numberOfMaterialsInShader*_mAlignOffset == 0){
        glBufferData(GL_SHADER_STORAGE_BUFFER, (_numberOfMaterialsInShader + scene.materials.size()) * _mAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (_numberOfMaterialsInShader + scene.materials.size()) * _mAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if(scene.materials.size() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for(const auto &mt : scene.materials){
            memcpy(ptr + _numberOfMaterialsInShader*_mAlignOffset + _mOffsets[0], &(mt.diffuse) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfMaterialsInShader*_mAlignOffset + _mOffsets[1], &(mt.specularity) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfMaterialsInShader*_mAlignOffset + _mOffsets[2], &(mt.emission) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfMaterialsInShader*_mAlignOffset + _mOffsets[3], &(mt.shininess) , sizeof(float));
            _numberOfMaterialsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_shManager->getShaderProgramID(ogl.getCompShaderProgName()), _mBlockIndex, 1);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }


    ////////////////////////////////////////////////////// LOADING LIGHTS //////////////////////////////////////////////////////
    p = malloc(_numberOfLightsInShader*_lAlignOffset);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ogl.getStorageBufferIDs()[2]);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, _numberOfLightsInShader*_lAlignOffset, p);
    if (_numberOfLightsInShader*_lAlignOffset == 0){
        glBufferData(GL_SHADER_STORAGE_BUFFER, scene.numberOfLights() * _lAlignOffset, NULL, GL_STATIC_DRAW);
    } else {
        glBufferData(GL_SHADER_STORAGE_BUFFER, (_numberOfLightsInShader + scene.numberOfLights()) * _lAlignOffset, p, GL_STATIC_DRAW);
        free(p);
    }

    if (scene.numberOfLights() != 0){
        GLubyte* ptr = (GLubyte*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);

        for(const auto &pl : scene.pointLights){
            memcpy(ptr + _numberOfLightsInShader*_lAlignOffset + _lOffsets[0], &(pl.position) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfLightsInShader*_lAlignOffset + _lOffsets[1], &(pl.color) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfLightsInShader*_lAlignOffset + _lOffsets[2], &(pl.attenuation) , sizeof(glm::vec4));
            _numberOfLightsInShader++;
        }

        for(const auto &dl : scene.directionalLights){
            memcpy(ptr + _numberOfLightsInShader*_lAlignOffset + _lOffsets[0], &(dl.position) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfLightsInShader*_lAlignOffset + _lOffsets[1], &(dl.color) , sizeof(glm::vec4));
            memcpy(ptr + _numberOfLightsInShader*_lAlignOffset + _lOffsets[2], &(dl.attenuation) , sizeof(glm::vec4));
            _numberOfLightsInShader++;
        }

        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        glShaderStorageBlockBinding(m_shManager->getShaderProgramID(ogl.getCompShaderProgName()), _lBlockIndex, 2);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    m_shManager->loadUniform_(ogl.getCompShaderProgName(), std::string("numObj"), _numberOfObjInShader);
    m_shManager->loadUniform_(ogl.getCompShaderProgName(), std::string("numLights"), _numberOfLightsInShader);
}