#include "TextureRenderer.h"

TextureRenderer::TextureRenderer(int width, int height, std::shared_ptr<Texture>& texture) : _width(width), _height(height), _texture(texture){
	_shaderProgName = "textureRenderer";
	initialize();
}

TextureRenderer::~TextureRenderer(){
	glDeleteBuffers(1, &_vertexUVBufferID);
}

void TextureRenderer::initialize(){
	assert(glGetError() == GL_NO_ERROR);
	
	glGenVertexArrays(1, &_VertexArrayID);
	glBindVertexArray(_VertexArrayID);
	glEnableVertexAttribArray(0);
	
	struct Vertex Vertices[]={
		{glm::vec4(-1,-1,0,1), glm::vec2(0,0)}, {glm::vec4(1,-1,0,1), glm::vec2(1,0)},  
		{glm::vec4(-1,1,0,1), glm::vec2(0,1)}, {glm::vec4(1,1,0,1), glm::vec2(1,1)} };

	glGenBuffers(1, &_vertexUVBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, _vertexUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	
	auto shManager = ShaderManager::getInstance();
	shManager->loadShader(".\\vs.glsl", "vertexShader", GL_VERTEX_SHADER);
	shManager->loadShader(".\\fs.glsl", "fragmentShader", GL_FRAGMENT_SHADER);

	shManager->createProgram(_shaderProgName);
	
	shManager->attachShader("vertexShader", _shaderProgName);
	shManager->attachShader("fragmentShader", _shaderProgName);

	shManager->linkProgram(_shaderProgName);
	shManager->useProgram(_shaderProgName);

	shManager->deleteShader("vertexShader");
	shManager->deleteShader("fragmentShader");
	
	assert(glGetError() == GL_NO_ERROR);
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

