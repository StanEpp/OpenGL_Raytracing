#include "OpenGLRaytracer.h"

OpenGLRaytracer::OpenGLRaytracer(std::shared_ptr<Texture>& texture, Camera& camera,
                                 int reflectionDepth, std::shared_ptr<ShaderManager>& shManager) :
    _texture(texture),
    m_shManager(shManager)
{
    _storageBufferIDs = new GLuint[3];
    _shaderProgName = "csRaytracer";

    createComputeshader(camera, reflectionDepth);
}


OpenGLRaytracer::~OpenGLRaytracer(){
    glDeleteBuffers(2, _storageBufferIDs);
    delete _storageBufferIDs;
}

void OpenGLRaytracer::createComputeshader(Camera& camera, int reflectionDepth){

    assert(glGetError() == GL_NO_ERROR);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture->getID());
    glBindImageTexture(0, _texture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, _texture->getFormat());

    if(glGetError() != GL_NO_ERROR){
        throw std::runtime_error("ERROR: Could not bind image texture!");
    }

    m_shManager->loadShader(".\\cs.glsl", "computeShader", GL_COMPUTE_SHADER);
    m_shManager->createProgram(_shaderProgName);
    m_shManager->attachShader("computeShader", _shaderProgName);
    m_shManager->linkProgram(_shaderProgName);
    m_shManager->deleteShader("computeShader");
    m_shManager->useProgram(_shaderProgName);

    m_shManager->loadUniform_(_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "camera.tanFovX", camera.getTanFovX());
    m_shManager->loadUniform_(_shaderProgName, "camera.tanFovY", camera.getTanFovY());
    m_shManager->loadUniform_(_shaderProgName, "width", _texture->getWidth());
    m_shManager->loadUniform_(_shaderProgName, "height", _texture->getHeight());
    m_shManager->loadUniform_(_shaderProgName, "reflectionDepth", reflectionDepth);

}

void OpenGLRaytracer::renderScene(Camera& camera, int width, int height, int reflectionDepth){
    glUseProgram(m_shManager->getShaderProgramID(_shaderProgName));

    m_shManager->loadUniform_(_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.f);
    m_shManager->loadUniform_(_shaderProgName, "reflectionDepth", reflectionDepth);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glDispatchCompute(width/WORK_GROUP_SIZE, height/WORK_GROUP_SIZE,1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    //glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

}

std::string OpenGLRaytracer::getCompShaderProgName()
{
    return _shaderProgName;
}

GLuint* OpenGLRaytracer::getStorageBufferIDs()
{
    return _storageBufferIDs;
}
