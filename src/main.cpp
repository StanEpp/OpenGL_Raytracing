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
        if (gl3wInit())
            throw std::runtime_error("Could not initialize gl3w!");
        if (!gl3wIsSupported(4, 3))
            throw std::runtime_error("OpenGL 4.3 not supported!");
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    } catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        return -1;
    }

    GLFWInput inputControl;
    inputControl.bindInputToWindow(*wnd);

    GLFWTimer timer;
    int maxFPS = settings.maxFPS;
    int width = settings.width;
    int height = settings.height;

    int frameCounter = 0, fps = 0;
    int reflectionDepth = settings.reflectionDepth;
    bool running = true;

    try{
        auto shManager = std::make_shared<ShaderManager>();
        auto camera = std::make_shared<Camera>(width, height, settings.fovY, settings.cameraSensitivity,
                                               glm::vec3(0,0,1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        auto tex = std::make_shared<Texture>(width, height, GL_RGBA8);
        auto texRenderer = std::make_shared<TextureRenderer>(width, height, tex, shManager);
        auto raytracer = std::make_shared<OpenGLRaytracer>(tex, *camera.get(), reflectionDepth, shManager);
        auto scene = std::make_shared<Scene>();

        {
            auto sceneReader = std::make_shared<SceneReader>();
            auto sceneLoader = std::make_shared<SceneLoader>(raytracer, shManager);

            for(int i = 1; i < argc; ++i){
                std::cout << "Loading Scene: " << argv[i] << std::endl;
                sceneReader->readScene(std::string(argv[i]).c_str(), (*scene.get()));
                std::cout << "Reading Scene Complete" << std::endl;
                sceneLoader->loadScene(scene, raytracer);
                std::cout << "Loading Scene Complete" << std::endl;
                scene->clear();
            }
        }

        assert(glGetError() == GL_NO_ERROR);

        while(running){
            if(timer.getTimeDiffWithoutActualization() > 1./static_cast<double>(maxFPS)){
                auto dt = timer.getTimeDiff();

                inputControl.updateInput();
                camera->update(inputControl, static_cast<float>(dt));

                raytracer->renderScene((*camera.get()), settings.width, settings.height, reflectionDepth);

                texRenderer->draw();

                wnd->swapBuffers();

                if(inputControl.isKeyPressed(GLFW_KEY_ESCAPE)){
                    running = false;
                }
                if(inputControl.isKeyPressedOnce(GLFW_KEY_KP_SUBTRACT)){
                    if (reflectionDepth > 0)
                        --reflectionDepth;
                    std::cout << "Reflection minus\n";
                }
                if(inputControl.isKeyPressedOnce(GLFW_KEY_KP_ADD)){
                    ++reflectionDepth;
                    std::cout << "Reflection plus\n";
                }
                frameCounter++;
                if(timer.getRefreshedTime() > 1.0){
                    fps = frameCounter;
                    frameCounter = 0;
                    timer.resetTime();
                }

                wnd->setWindowTitle(std::to_string(fps).c_str());
            }
        }

    } catch(std::exception&e ) {
        std::cout << e.what() <<std::endl;
    }

    return 0;
}
