#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cassert>
#include <memory>
#include "GLFWWindow.h"
#include "ShaderManager.hpp"
#include "TextureRenderer.h"
#include "OpenGLRaytracer.h"
#include "GLFWTimer.h"
#include "SceneReader.h"
#include "SceneLoader.h"
#include "GLFWInput.hpp"
#include "ConfigLoader.h"

int main(int argc, char* argv[]){
    ConfigLoader configLoader("settings.txt");
    const auto settings = configLoader.settings();

    std::shared_ptr<GLFWWindow> wnd;

    try {
        wnd = std::make_shared<GLFWWindow>(settings.width, settings.height,
                                           "RayTracer",
                                           settings.fullscreen);
        if (gl3wInit()) throw std::runtime_error("Could not initialize gl3w!");
        if (!gl3wIsSupported(4, 3)) throw std::runtime_error("OpenGL 4.3 not supported!");
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    } catch(std::exception& e){
        std::cout << e.what() << std::endl;
        return -1;
    }

    GLFWInput input;
    GLFWTimer timer;
    int maxFPS = settings.maxFPS;
    int width = settings.width;
    int height = settings.height;

    int frameCounter = 0, fps = 0;
    int reflectionDepth = settings.reflectionDepth;
    bool running = true;
    char i = 0;

    try{
        auto shManager = std::make_shared<ShaderManager>();
        auto camera = std::make_shared<Camera>(width, height, settings.fovY,
                                               glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0));

        auto tex = std::make_shared<Texture>(width, height, GL_RGBA8);
        auto texRenderer = std::make_shared<TextureRenderer>(width, height, tex, shManager);
        auto ort = std::make_shared<OpenGLRaytracer>(tex, *camera.get(), reflectionDepth, shManager);
        auto inputControl = std::make_shared<GLFWInput>();
        auto scene = std::make_shared<Scene>();
        auto sceneReader = std::make_shared<SceneReader>();
        auto sceneLoader = std::make_shared<SceneLoader>(ort, shManager);

        for(int i = 1; i < argc; ++i){
            std::cout << "Loading Scene: " << argv[i] << std::endl;
            sceneReader->readScene((std::string(".\\")+std::string(argv[i])).c_str(), (*scene.get()));
            std::cout << "Reading Scene Complete" << std::endl;
            sceneLoader->loadScene(*scene.get(), *ort.get());
            std::cout << "Loading Scene Complete" << std::endl;
            scene->clear();
        }
        assert(glGetError() == GL_NO_ERROR);

        while(running){
            assert(glGetError() == GL_NO_ERROR);
            if(timer.getTimeDiffWithoutActualization() > static_cast<double>(1.0)/static_cast<double>(maxFPS)){
                timer.getTimeDiff();

                inputControl->updateInput();
                camera->update();

                ort->renderScene((*camera.get()), settings.width, settings.height, reflectionDepth);

                texRenderer->draw();

                wnd->swapBuffers();

                if(inputControl->isKeyPressed(GLFW_KEY_ESCAPE)){
                    running = false;
                }
                frameCounter++;
                if(timer.getRefreshedTime() > 1.0){
                    fps = frameCounter;
                    frameCounter = 0;
                    timer.resetTime();
                }

                wnd->setWindowTitle(std::to_string(fps).c_str());

            }
            assert(glGetError() == GL_NO_ERROR);

        }

    } catch(std::exception&e ) {
        std::cout << e.what() <<std::endl;
    }

    return 0;
}
