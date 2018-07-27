#include "TextureRenderer.h"
#include "Vertex.h"

TextureRenderer::TextureRenderer(int width, int height,
                                 const std::shared_ptr<Texture> &texture,
                                 const std::shared_ptr<ShaderManager> &shManager) :
    m_width(width),
    m_height(height),
    m_texture(texture),
    m_shManager(shManager)
{
    m_shaderProgName = "textureRenderer";
    initialize();
}

TextureRenderer::~TextureRenderer()
{
    glDeleteBuffers(1, &m_vertexUVBufferID);
}

void TextureRenderer::initialize()
{
    assert(glGetError() == GL_NO_ERROR);

    glGenVertexArrays(1, &m_VertexArrayID);
    glBindVertexArray(m_VertexArrayID);
    glEnableVertexAttribArray(0);

    struct Vertex Vertices[]={
        {glm::vec4(-1,-1,0,1), glm::vec2(0,0)}, {glm::vec4(1,-1,0,1), glm::vec2(1,0)},
        {glm::vec4(-1,1,0,1), glm::vec2(0,1)}, {glm::vec4(1,1,0,1), glm::vec2(1,1)} };

    glGenBuffers(1, &m_vertexUVBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexUVBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    m_shManager->loadShader("vs.glsl", "vertexShader", GL_VERTEX_SHADER);
    m_shManager->loadShader("fs.glsl", "fragmentShader", GL_FRAGMENT_SHADER);

    m_shManager->createProgram(m_shaderProgName);

    m_shManager->attachShader("vertexShader", m_shaderProgName);
    m_shManager->attachShader("fragmentShader", m_shaderProgName);

    m_shManager->linkProgram(m_shaderProgName);
    m_shManager->useProgram(m_shaderProgName);

    m_shManager->deleteShader("vertexShader");
    m_shManager->deleteShader("fragmentShader");

    assert(glGetError() == GL_NO_ERROR);
}

void TextureRenderer::draw()
{
    glUseProgram(m_shManager->getShaderProgramID(m_shaderProgName));

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexUVBufferID);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec4));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

