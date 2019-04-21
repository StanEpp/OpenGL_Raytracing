#pragma once

#include <string>
#include <memory>
#include "Scene.hpp"
#include "ShaderManager.hpp"

class SceneManager
{
public:
    void uploadScenes(const std::vector<std::string> &filepaths, ShaderManager& shManager, GLuint computeShaderID, GLuint* computerShaderstorageBufferIDs);

private:
    void initialize(GLuint computerShaderID);
    void readScene(const std::string &filepath);
    void uploadScene(ShaderManager& shManager, GLuint computeShaderID, GLuint* computerShaderstorageBufferIDs);

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

    Scene m_scene;
};