#pragma once

#include <string>
#include <memory>
#include "Scene.h"

class OpenGLRaytracer;
class ShaderManager;

class SceneManager
{
public:
    SceneManager(const std::shared_ptr<OpenGLRaytracer> &raytracer,
                 const std::shared_ptr<ShaderManager> &shManager);

    void uploadScenes(const std::vector<std::string> &filepaths);

private:
    void initialize(const std::shared_ptr<OpenGLRaytracer> &raytracer);
    void readScene(const std::string &filepath);
    void uploadScene();

    unsigned int m_numObjInShader = 0;
    unsigned int m_numMaterialsInShader = 0;
    unsigned int m_numLightsInShader = 0;

    int m_oBlockIndex = 0;
    int m_mBlockIndex = 0;
    int m_lBlockIndex = 0;

    int m_oAlignOffset = 0;
    int m_mAlignOffset = 0;
    int m_lAlignOffset = 0;

    int *m_oOffsets = nullptr;
    int *m_mOffsets = nullptr;
    int *m_lOffsets = nullptr;

    std::shared_ptr<OpenGLRaytracer> m_raytracer;
    std::shared_ptr<ShaderManager> m_shManager;
    Scene m_scene;
};