#include "TextureRenderer.h"
#include <glm\ext.hpp>


TextureRenderer::TextureRenderer(int width, int height, boost::shared_ptr<Texture>& texture) : _width(width), _height(height), _texture(texture){

	_shaderProgName = "textureRenderer";
	initialize();
}

TextureRenderer::~TextureRenderer(){
	glDeleteBuffers(1, &_vertexUVBufferID);
}

void TextureRenderer::initialize(){

	struct Vertex Vertices[]={
		{glm::vec4(-1,-1,0,1), glm::vec2(0,0)}, {glm::vec4(1,-1,0,1), glm::vec2(1,0)},  
		{glm::vec4(-1,1,0,1), glm::vec2(0,1)}, {glm::vec4(1,1,0,1), glm::vec2(1,1)} };

	glGenBuffers(1, &_vertexUVBufferID);
 	glBindBuffer(GL_ARRAY_BUFFER, _vertexUVBufferID);
 	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	ShaderManager::getInstance()->loadShader(".\\vs.glsl", "vertexShader", GL_VERTEX_SHADER);
	ShaderManager::getInstance()->loadShader(".\\fs.glsl", "fragmentShader", GL_FRAGMENT_SHADER);

	ShaderManager::getInstance()->createProgram(_shaderProgName);
	
	ShaderManager::getInstance()->attachShader("vertexShader", _shaderProgName);
	ShaderManager::getInstance()->attachShader("fragmentShader", _shaderProgName);

	ShaderManager::getInstance()->linkProgram(_shaderProgName);
	ShaderManager::getInstance()->useProgram(_shaderProgName);

	ShaderManager::getInstance()->deleteShader("vertexShader");
	ShaderManager::getInstance()->deleteShader("fragmentShader");
}

void TextureRenderer::draw(){
	glUseProgram(ShaderManager::getInstance()->getShaderProgramID(_shaderProgName));

	glBindBuffer(GL_ARRAY_BUFFER, _vertexUVBufferID);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec4));
 
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 
 
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

