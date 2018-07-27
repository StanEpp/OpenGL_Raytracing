#pragma once

#include "OpenGLRaytracer.h"
#include "Primitives.h"
#include "Scene.h"
#include "ShaderManager.hpp"

class SceneLoader
{
public:
    SceneLoader() = delete;
    SceneLoader(const std::shared_ptr<OpenGLRaytracer> &raytracer, const std::shared_ptr<ShaderManager> &shManager);
    ~SceneLoader();

    void loadScene(Scene&, OpenGLRaytracer&);

private:
    int m_numberOfObjInShader, m_numberOfMaterialsInShader, m_numberOfLightsInShader;
    int m_materialElements, m_objectElements, m_lightElements;
    int m_oBlockSize, m_oBlockIndex, m_mBlockSize, m_mBlockIndex, m_lBlockIndex, m_lBlockSize;
    int m_oAlignOffset, m_mAlignOffset, m_lAlignOffset;
    int *m_oIndices, *m_mIndices, *m_lIndices, *m_oOffsets, *m_mOffsets, *m_lOffsets;
    GLuint m_copyBufferIDs[2];
    GLenum m_props[1], m_props2[1];
    std::shared_ptr<ShaderManager> m_shManager;

    void initialize(const std::shared_ptr<OpenGLRaytracer> &raytracer);
};