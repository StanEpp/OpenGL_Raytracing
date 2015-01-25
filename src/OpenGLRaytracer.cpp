#include "OpenGLRaytracer.h"

OpenGLRaytracer::OpenGLRaytracer(std::shared_ptr<Texture>& texture, Camera& camera, int reflectionDepth) : _texture(texture) {

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
	
	auto shManager = ShaderManager::getInstance();
	
	shManager->loadShader(".\\cs.glsl", "computeShader", GL_COMPUTE_SHADER);
	shManager->createProgram(_shaderProgName);
	shManager->attachShader("computeShader", _shaderProgName);
	shManager->linkProgram(_shaderProgName);
	shManager->deleteShader("computeShader");
	shManager->useProgram(_shaderProgName);

	shManager->loadVec4Uniform(_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0);
	shManager->loadVec4Uniform(_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.0);
	shManager->loadVec4Uniform(_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.0);
	shManager->loadVec4Uniform(_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.0);
	shManager->loadFloatUniform(_shaderProgName, "camera.tanFovX", camera.getTanFovX());
	shManager->loadFloatUniform(_shaderProgName, "camera.tanFovY", camera.getTanFovY());
	shManager->loadUintUniform(_shaderProgName, "width", _texture->getWidth());
	shManager->loadUintUniform(_shaderProgName, "height", _texture->getHeight());
	shManager->loadUintUniform(_shaderProgName, "reflectionDepth", reflectionDepth);

}

void OpenGLRaytracer::renderScene(Camera& camera, int width, int height, int reflectionDepth){
	auto shManager = ShaderManager::getInstance();

	glUseProgram(shManager->getShaderProgramID(_shaderProgName));

	shManager->loadVec4Uniform(_shaderProgName, "camera.pos", camera.getPos().x, camera.getPos().y, camera.getPos().z, 0);
	shManager->loadVec4Uniform(_shaderProgName, "camera.dir", camera.getDir().x, camera.getDir().y, camera.getDir().z, 0.0);
	shManager->loadVec4Uniform(_shaderProgName, "camera.yAxis", camera.getYAxis().x, camera.getYAxis().y, camera.getYAxis().z, 0.0);
	shManager->loadVec4Uniform(_shaderProgName, "camera.xAxis", camera.getXAxis().x, camera.getXAxis().y, camera.getXAxis().z, 0.0);
	shManager->loadUintUniform(_shaderProgName, "reflectionDepth", reflectionDepth);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glDispatchCompute(width/WORK_GROUP_SIZE, height/WORK_GROUP_SIZE,1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);

}

std::string OpenGLRaytracer::getCompShaderProgName(){
	return _shaderProgName;
}

GLuint* OpenGLRaytracer::getStorageBufferIDs(){
	return _storageBufferIDs;
}
