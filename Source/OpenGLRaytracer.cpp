#include "OpenGLRaytracer.h"

OpenGLRaytracer::OpenGLRaytracer(boost::shared_ptr<Texture>& texture, Camera& camera, int reflectionDepth) : _texture(texture) {

	_storageBufferIDs = new GLuint[3];
	_shaderProgName = "csRaytracer";

	createComputeshader(camera, reflectionDepth);
}


OpenGLRaytracer::~OpenGLRaytracer(){
	glDeleteBuffers(2, _storageBufferIDs);
	delete _storageBufferIDs;
}

void OpenGLRaytracer::createComputeshader(Camera& camera, int reflectionDepth){

	glGetError();
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture->getID());
	glBindImageTexture(0, _texture->getID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, _texture->getFormat());

	if(glGetError() != GL_NO_ERROR){
		throw std::exception("ERROR: Could not bind image texture!");
	}

	ShaderManager::getInstance()->loadShader(".\\cs.glsl", "computeShader", GL_COMPUTE_SHADER);
	ShaderManager::getInstance()->createProgram(_shaderProgName);
	ShaderManager::getInstance()->attachShader("computeShader", _shaderProgName);
	ShaderManager::getInstance()->linkProgram(_shaderProgName);
	ShaderManager::getInstance()->deleteShader("computeShader");
	ShaderManager::getInstance()->useProgram(_shaderProgName);

	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0);
	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.0);
	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.0);
	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.0);
	ShaderManager::getInstance()->loadFloatUniform(_shaderProgName, "camera.tanFovX", camera.getTanFovX());
	ShaderManager::getInstance()->loadFloatUniform(_shaderProgName, "camera.tanFovY", camera.getTanFovY());
	ShaderManager::getInstance()->loadUintUniform(_shaderProgName, "width", _texture->getWidth());
	ShaderManager::getInstance()->loadUintUniform(_shaderProgName, "height", _texture->getHeight());
	ShaderManager::getInstance()->loadUintUniform(_shaderProgName, "reflectionDepth", reflectionDepth);

}

void OpenGLRaytracer::renderScene(Camera& camera, int width, int height, int reflectionDepth){

	glUseProgram(ShaderManager::getInstance()->getShaderProgramID(_shaderProgName));

	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0);
	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.0);
	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.0);
	ShaderManager::getInstance()->loadVec4Uniform(_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.0);
	ShaderManager::getInstance()->loadUintUniform(_shaderProgName, "reflectionDepth", reflectionDepth);

#ifndef __CPU
	glDispatchCompute(width/WORK_GROUP_SIZE, height/WORK_GROUP_SIZE,1);

	glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
#endif

}

std::string OpenGLRaytracer::getCompShaderProgName(){
	return _shaderProgName;
}

GLuint* OpenGLRaytracer::getStorageBufferIDs(){
	return _storageBufferIDs;
}
