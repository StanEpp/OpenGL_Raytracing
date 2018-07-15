#pragma once

#include "OpenGLRaytracer.h"
#include "Primitives.h"
#include "Scene.h"
#include "ShaderManager.hpp"

class SceneLoader{
private:
    int _numberOfObjInShader, _numberOfMaterialsInShader, _numberOfLightsInShader;
    int _materialElements, _objectElements, _lightElements;
    int _oBlockSize, _oBlockIndex, _mBlockSize, _mBlockIndex, _lBlockIndex, _lBlockSize;
    int _oAlignOffset, _mAlignOffset, _lAlignOffset;
    int *_oIndices, *_mIndices, *_lIndices, *_oOffsets, *_mOffsets, *_lOffsets;
    GLuint _copyBufferIDs[2];
    GLenum _props[1], _props2[1];
    std::shared_ptr<ShaderManager> m_shManager;

    void initialize(const std::shared_ptr<OpenGLRaytracer> &raytracer);

public:
    SceneLoader() = delete;

    SceneLoader(const std::shared_ptr<OpenGLRaytracer> &raytracer, const std::shared_ptr<ShaderManager> &shManager);

    ~SceneLoader(){
        delete[] _oIndices;
        delete[] _mIndices;
        delete[] _lIndices;
        delete[] _oOffsets;
        delete[] _mOffsets;
        delete[] _lOffsets;
    }

    void loadScene(Scene&, OpenGLRaytracer&);
};