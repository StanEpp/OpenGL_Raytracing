#ifndef _GLEW_
#define _GLEW_
	#include <GL\glew.h>
	#include <GLFW\glfw3.h>
#endif

#include <iostream>
#include <cassert>
#include <memory>
#include <XMLParser.h>
#include "GLFWWindow.h"
#include "ShaderManager.h"
#include "GLFWInput.h"
#include "TextureRenderer.h"
#include "OpenGLRaytracer.h"
#include "GLFWTimer.h"
#include "SceneReader.h"
#include "SceneLoader.h"
#include "InputControl.h"

void loadDefaultValues(XMLParser* xml);


int main(int argc, char* argv[]){
	XMLParser* xml = new XMLParser();

	try{
		xml->loadXML(".\\settings.xml");
	} catch(std::exception& e){
		std::cout << e.what() <<std::endl;
		std::cout << "Default values will be applied" <<std::endl;
		loadDefaultValues(xml);

		xml->writeToXML(".\\settings.xml");
	}

	GLFWWindow* wnd;

	try{
		wnd = new GLFWWindow(xml->get<int>("Window", "width"),
							 xml->get<int>("Window", "height"),
							 "RayTracer",
							 xml->get<bool>("Window", "windowed"));
	} catch(std::exception& e){
		std::cout << e.what() << std::endl;
		std::cin.get();
		delete xml;
		
		return -1;
	}

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK){
		std::cout << "Could not initialize GLEW!" <<std::endl;
		std::cin.get();

		delete wnd;
		delete xml;

		return -1;
	} else {
		std::cout << "GLEW Version: " << glewGetString(GLEW_VERSION) <<std::endl;
		assert(glGetError() == GL_NO_ERROR);
	}
	
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) <<std::endl;


	GLFWInput* input = new GLFWInput();
	GLFWTimer* timer = new GLFWTimer();
	int maxFPS = xml->get<int>("Window", "maxFPS");
	int width = xml->get<int>("Window", "width");
	int height = xml->get<int>("Window", "height");

	std::shared_ptr<TextureRenderer>	texRenderer;
	std::shared_ptr<OpenGLRaytracer>	ort;
	std::shared_ptr<Texture>			tex;
	std::shared_ptr<Scene>				scene;
	std::shared_ptr<SceneReader>		sceneReader;
	std::shared_ptr<SceneLoader>		sceneLoader;
	std::shared_ptr<InputControl>		inputControl;
	std::shared_ptr<Camera>				camera;

	int frameCounter = 0, fps = 0;
	int reflectionDepth = xml->get<int>("Raytracing", "reflectionDepth"); 
	bool running = true;
	char i = 0;

	try{
		camera = std::shared_ptr<Camera>(
			new Camera(
				xml->get<int>("Window", "width"), xml->get<int>("Window", "height"), xml->get<float>("Camera", "fovY"), 
				glm::vec3(xml->get<float>("Camera", "posX"), xml->get<float>("Camera", "posY"),xml->get<float>("Camera", "posZ")), 
				glm::vec3(xml->get<float>("Camera", "dirX"), xml->get<float>("Camera", "dirY"), xml->get<float>("Camera", "dirZ")),
				glm::vec3(xml->get<float>("Camera", "upX"), xml->get<float>("Camera", "upY"), xml->get<float>("Camera", "upZ"))
				)
			);

		tex.reset( new Texture(xml->get<int>("Window", "width"), xml->get<int>("Window", "height"), GL_RGBA8));
		texRenderer.reset( new TextureRenderer(xml->get<int>("Window", "width"), xml->get<int>("Window", "height"), tex));
		ort.reset( new OpenGLRaytracer(tex, (*camera.get()), reflectionDepth));
		inputControl.reset( new InputControl(xml->get<float>("Camera", "velocityRotate"), xml->get<float>("Camera", "velocityTranslate")));
		scene.reset( new Scene());
		sceneReader.reset( new SceneReader());
		sceneLoader.reset( new SceneLoader((*ort.get())));

		for(int i = 1; i < argc; ++i){
			std::cout << "LOADING SCENE: " << argv[i] <<std::endl;
			sceneReader->readScene((std::string(".\\")+std::string(argv[i])).c_str(), (*scene.get()));
			std::cout << "READING SCENE COMPLETE" <<std::endl;
			sceneLoader->loadScene((*scene.get()), (*ort.get()));
			std::cout << "LOADING SCENE COMPLETE" <<std::endl;
			scene->clear();
		}
		assert(glGetError() == GL_NO_ERROR);
		std::cout << "SCENELOADING COMPLETE!" <<std::endl;
		
		while(running){
			assert(glGetError() == GL_NO_ERROR);	
			if(timer->getTimeDiffWithoutActualization() > static_cast<double>(1.0)/static_cast<double>(maxFPS)){
				timer->getTimeDiff();

				inputControl->processInput((*camera.get()), reflectionDepth, wnd->getGLFWwindow());
				camera->update();

				ort->renderScene((*camera.get()), width, height, reflectionDepth);

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

				wnd->setWindowTitle(boost::lexical_cast<std::string, int>(fps).c_str());

			}
			
			assert(glGetError() == GL_NO_ERROR);

		}
		
	} catch(std::exception&e ){
		std::cout << e.what() <<std::endl;
		std::cin.get();
	}
	
	delete timer;
	delete input;
	delete wnd;
}

void loadDefaultValues(XMLParser* xml){
	xml->add("Window", "width", 1024);
	xml->add("Window", "height", 768);
	xml->add("Window", "maxFPS", 60);
	xml->add("Window", "windowed", true);
	xml->add("Camera", "fovY", 45.0f);
	xml->add("Camera", "velocityRotate", 0.5f);
	xml->add("Camera", "velocityTranslate", 0.2f);
	xml->add("Camera", "posX", 0.0f);
	xml->add("Camera", "posY", 0.0f);
	xml->add("Camera", "posZ", 0.0f);
	xml->add("Camera", "upX", 0.0f);
	xml->add("Camera", "upY", 1.0f);
	xml->add("Camera", "upZ", 0.0f);
	xml->add("Camera", "dirX", 0.0f);
	xml->add("Camera", "dirY", 0.0f);
	xml->add("Camera", "dirZ", 1.0f);
	xml->add("Raytracing", "reflectionDepth", 5);
}
