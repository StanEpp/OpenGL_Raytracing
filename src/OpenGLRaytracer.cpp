#include "OpenGLRaytracer.h"

OpenGLRaytracer::OpenGLRaytracer(std::shared_ptr<Texture>& textureToRender, Camera& camera,
                                 unsigned int reflectionDepth, std::shared_ptr<ShaderManager>& shManager) :
    m_texture(textureToRender),
    m_shManager(shManager)
{
    m_storageBufferIDs = new GLuint[3];
    m_shaderProgName = "csRaytracer";

    createComputeshader(camera, reflectionDepth);
}


OpenGLRaytracer::~OpenGLRaytracer()
{
    glDeleteBuffers(2, m_storageBufferIDs);
    delete m_storageBufferIDs;
}

void OpenGLRaytracer::createComputeshader(Camera& camera, unsigned int reflectionDepth)
{
    assert(glGetError() == GL_NO_ERROR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->getID());
    glBindImageTexture(0, m_texture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, m_texture->getFormat());

    if(glGetError() != GL_NO_ERROR){
        throw std::runtime_error("ERROR: Could not bind image texture!");
    }

    m_shManager->loadShader("shader/cs.glsl", "computeShader", GL_COMPUTE_SHADER);
    m_shManager->createProgram(m_shaderProgName);
    m_shManager->attachShader("computeShader", m_shaderProgName);
    m_shManager->linkProgram(m_shaderProgName);
    m_shManager->deleteShader("computeShader");
    m_shManager->useProgram(m_shaderProgName);

    m_shManager->loadUniform_(m_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "camera.tanFovX", camera.getTanFovX());
    m_shManager->loadUniform_(m_shaderProgName, "camera.tanFovY", camera.getTanFovY());
    m_shManager->loadUniform_(m_shaderProgName, "width", m_texture->getWidth());
    m_shManager->loadUniform_(m_shaderProgName, "height", m_texture->getHeight());
    m_shManager->loadUniform_(m_shaderProgName, "reflectionDepth", reflectionDepth);

}

void OpenGLRaytracer::renderScene(Camera& camera, unsigned int width, unsigned int height, unsigned int reflectionDepth)
{
    glUseProgram(m_shManager->getShaderProgramID(m_shaderProgName));

    m_shManager->loadUniform_(m_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.f);
    m_shManager->loadUniform_(m_shaderProgName, "reflectionDepth", reflectionDepth);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glDispatchCompute(width/WORK_GROUP_SIZE, height/WORK_GROUP_SIZE,1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    //glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

}

std::string OpenGLRaytracer::getCompShaderProgName()
{
    return m_shaderProgName;
}

GLuint* OpenGLRaytracer::getStorageBufferIDs()
{
    return m_storageBufferIDs;
}
