#ifndef _GLEW_
#define _GLEW_
	#include <GL/gl3w.h>
	#include <GLFW/glfw3.h>
#endif

#include <iostream>
#include <cassert>
#include <memory>
#include "GLFWWindow.h"
#include "ShaderManager.h"
#include "GLFWInput.h"
#include "TextureRenderer.h"
#include "OpenGLRaytracer.h"
#include "GLFWTimer.h"
#include "SceneReader.h"
#include "SceneLoader.h"
#include "InputControl.h"
#include "SettingsLoader.h"

int main(int argc, char* argv[]){
	SettingsLoader settingsLoader("settings.txt");
	const auto& settings = settingsLoader.settings();

	GLFWWindow* wnd;

	try {
		wnd = new GLFWWindow(settings.width, settings.height,
							"RayTracer",
							settings.windowed);
		if (gl3wInit()) throw std::runtime_error("Could not initialize gl3w!");
		if (!gl3wIsSupported(4, 3)) throw std::runtime_error("OpenGL 4.3 not supported!");
		std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
		std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	} catch(std::exception& e){
		std::cout << e.what() << std::endl;
		return -1;
	}

	GLFWInput* input = new GLFWInput();
	GLFWTimer* timer = new GLFWTimer();
	int maxFPS = settings.maxFPS;
	int width = settings.width;
	int height = settings.height;

	std::shared_ptr<TextureRenderer>	texRenderer;
	std::shared_ptr<OpenGLRaytracer>	ort;
	std::shared_ptr<Texture>			tex;
	std::shared_ptr<Scene>				scene;
	std::shared_ptr<SceneReader>		sceneReader;
	std::shared_ptr<SceneLoader>		sceneLoader;
	std::shared_ptr<InputControl>		inputControl;
	std::shared_ptr<Camera>				camera;

	int frameCounter = 0, fps = 0;
	int reflectionDepth = settings.reflectionDepth; 
	bool running = true;
	char i = 0;

	try{
		camera = std::shared_ptr<Camera>(
			new Camera(
				settings.width, settings.height, settings.fovY,
				glm::vec3(settings.posX, settings.posY, settings.posZ),
				glm::vec3(settings.dirX, settings.dirY, settings.dirZ),
				glm::vec3(settings.upX, settings.upY, settings.upZ)
				)
			);

		tex.reset( new Texture(settings.width, settings.height, GL_RGBA8));
		texRenderer.reset( new TextureRenderer(settings.width, height, tex));
		ort.reset( new OpenGLRaytracer(tex, (*camera.get()), reflectionDepth));
		inputControl.reset( new InputControl(settings.velocityRotate, settings.velocityTranslate));
		scene.reset( new Scene());
		sceneReader.reset( new SceneReader());
		sceneLoader.reset( new SceneLoader((*ort.get())));

		for(int i = 1; i < argc; ++i){
			std::cout << "Loading Scene: " << argv[i] <<std::endl;
			sceneReader->readScene((std::string(".\\")+std::string(argv[i])).c_str(), (*scene.get()));
			std::cout << "Reading Scene Complete" <<std::endl;
			sceneLoader->loadScene((*scene.get()), (*ort.get()));
			std::cout << "Loading Scene Complete" <<std::endl;
			scene->clear();
		}
		assert(glGetError() == GL_NO_ERROR);
		
		while(running){
			assert(glGetError() == GL_NO_ERROR);	
			if(timer->getTimeDiffWithoutActualization() > static_cast<double>(1.0)/static_cast<double>(maxFPS)){
				timer->getTimeDiff();

				inputControl->processInput((*camera.get()), reflectionDepth, wnd->getGLFWwindow());
				camera->update();

				ort->renderScene((*camera.get()), settings.width, settings.height, reflectionDepth);

				texRenderer->draw();

				wnd->swapBuffers();

				if(input->isKeyPressed(GLFW_KEY_ESCAPE, wnd->getGLFWwindow())){
					running = false;
				}
				frameCounter++;
				if(timer->getRefreshedTime() > 1.0){
					fps = frameCounter;
					frameCounter = 0;
					timer->resetTime();
				}

				wnd->setWindowTitle(std::to_string(fps).c_str());

			}
			
			assert(glGetError() == GL_NO_ERROR);

		}
		
	} catch(std::exception&e ){
		std::cout << e.what() <<std::endl;
	}
	
	delete timer;
	delete input;
	delete wnd;

	return 0;
}
