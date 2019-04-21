#include "Screenquad.hpp"
#include <glm/glm.hpp>

struct Vertex{
    glm::vec4 pos;
    glm::vec2 texCoor;
};

Screenquad::~Screenquad()
{
    glDeleteBuffers(1, &m_vertexUVBufferID);
}

Screenquad::Screenquad(ShaderManager& shManager)
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

    shManager.compileShader(R"(
    #version 430

    layout(location=0) in vec4 currentPos;
    layout(location=1) in vec2 texPos;
    out vec2 ex_TexCoor;

    void main(void)
    {
        gl_Position = currentPos;
        ex_TexCoor = texPos;
    }
    )"
    , "vertexShader", GL_VERTEX_SHADER);

    shManager.compileShader(R"(
    #version 430

    in vec2 ex_TexCoor;
    out vec4 color;

    uniform sampler2D texture;

    void main(void){
        color = texture2D(texture, ex_TexCoor);
    }
    )",
    "fragmentShader", GL_FRAGMENT_SHADER);

    const std::string shaderProgName = "screenQuadProg";

    m_shaderProgramID = shManager.createProgram(shaderProgName);

    shManager.attachShader("vertexShader", shaderProgName);
    shManager.attachShader("fragmentShader", shaderProgName);

    shManager.linkProgram(shaderProgName);
    shManager.useProgram(shaderProgName);

    shManager.deleteShader("vertexShader");
    shManager.deleteShader("fragmentShader");

    assert(glGetError() == GL_NO_ERROR);
}

void Screenquad::draw(Texture& textureToRender)
{
    glUseProgram(m_shaderProgramID);
    // glActiveTexture(GL_TEXTURE0 + textureToRender.bindPoint());

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexUVBufferID);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec4));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}