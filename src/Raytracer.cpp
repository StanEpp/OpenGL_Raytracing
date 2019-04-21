#include "Raytracer.hpp"

#include <limits>

#define WORK_GROUP_SIZE 16


Raytracer::Raytracer(const std::string& settingsFile, const std::vector<std::string> &sceneFiles) :
    m_settings(ConfigLoader(settingsFile).settings()),
    m_window(std::make_unique<GLFWWindow>(m_settings.width, m_settings.height, "RayTracer", m_settings.fullscreen)),
    m_shManager(),
    m_renderedToTexture(m_settings.width, m_settings.height),
    m_screenquad(m_shManager),
    m_camera(m_settings.width, m_settings.height, m_settings.fovY, m_settings.cameraSensitivity, glm::vec3(0, 1, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0))
{
    std::cout << "Test1\n";
    m_inputControl.bindInputToWindow(*m_window);
    std::cout << "Test4\n";
    m_storageBufferIDs = new GLuint[3];
    createComputeShader();

    std::cout << "Test5\n";

    SceneManager sceneManager;
    sceneManager.uploadScenes(sceneFiles, m_shManager, m_computeShaderID, m_storageBufferIDs);
}

Raytracer::~Raytracer()
{
    glDeleteBuffers(2, m_storageBufferIDs);
    delete m_storageBufferIDs;
}

void Raytracer::createComputeShader()
{
    assert(glGetError() == GL_NO_ERROR);

    const auto shaderProgName = "csRaytracer";

    m_shManager.loadShader("shader/cs.glsl", "computeShader", GL_COMPUTE_SHADER);
    m_computeShaderID = m_shManager.createProgram(shaderProgName);
    m_shManager.attachShader("computeShader", shaderProgName);
    m_shManager.linkProgram(shaderProgName);
    m_shManager.deleteShader("computeShader");
    m_shManager.useProgram(shaderProgName);

    m_shManager.loadUniform_(m_computeShaderID, "camera.pos", m_camera.pos().x, m_camera.pos().y, m_camera.pos().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "camera.dir", m_camera.lookDir().x, m_camera.lookDir().y, m_camera.lookDir().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "camera.yAxis", m_camera.up().x, m_camera.up().y, m_camera.up().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "camera.xAxis", m_camera.right().x, m_camera.right().y, m_camera.right().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "camera.tanFovX", m_camera.fovX());
    m_shManager.loadUniform_(m_computeShaderID, "camera.tanFovY", m_camera.fovY());
    m_shManager.loadUniform_(m_computeShaderID, "width", m_renderedToTexture.width());
    m_shManager.loadUniform_(m_computeShaderID, "height", m_renderedToTexture.height());
    m_shManager.loadUniform_(m_computeShaderID, "reflectionDepth", m_settings.reflectionDepth);

    glGenBuffers(3, m_storageBufferIDs);
}

void Raytracer::raytraceScene()
{
    glUseProgram(m_computeShaderID);

    m_shManager.loadUniform_(m_computeShaderID, "camera.pos", m_camera.pos().x, m_camera.pos().y, m_camera.pos().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "camera.dir", m_camera.lookDir().x, m_camera.lookDir().y, m_camera.lookDir().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "camera.yAxis", m_camera.up().x, m_camera.up().y, m_camera.up().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "camera.xAxis", m_camera.right().x, m_camera.right().y, m_camera.right().z, 0.f);
    m_shManager.loadUniform_(m_computeShaderID, "reflectionDepth", m_settings.reflectionDepth);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glDispatchCompute(m_settings.width/WORK_GROUP_SIZE, m_settings.height/WORK_GROUP_SIZE,1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    //glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT);
}

void Raytracer::run()
{
    uint32_t frameCounter = 0, fps = 0;
    bool running = true;
    GLFWTimer timer;

    while(running){
        if(timer.getTimeDiffWithoutActualization() > 1./static_cast<double>(m_settings.maxFPS)){
            auto dt = timer.getTimeDiff();

            m_inputControl.updateInput();
            m_camera.update(m_inputControl, static_cast<float>(dt));

            raytraceScene();

            m_screenquad.draw(m_renderedToTexture);

            m_window->swapBuffers();

            if(m_inputControl.isKeyPressed(GLFW_KEY_ESCAPE)){
                running = false;
            }
            if(m_inputControl.isKeyPressedOnce(GLFW_KEY_KP_SUBTRACT)){
                if (m_settings.reflectionDepth > 0)
                    --m_settings.reflectionDepth;
            }
            if(m_inputControl.isKeyPressedOnce(GLFW_KEY_KP_ADD)){
                ++m_settings.reflectionDepth;
            }
            frameCounter++;
            if(timer.getRefreshedTime() > 1.0){
                fps = frameCounter;
                frameCounter = 0;
                timer.resetTime();
            }

            m_window->setWindowTitle(std::to_string(fps).c_str());
        }
    }

}