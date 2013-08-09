#include "InputControl.h"


void InputControl::processInput(Camera& cam, int& reflectionDepth){
	if(_input->isKeyPressed('W')){
		_tz = _velocityTranslate;
	}

	if(_input->isKeyPressed('S')){
		_tz = -_velocityTranslate;
	}


	if(_input->isKeyPressed('D')){
		_tx = _velocityTranslate;
	}


	if(_input->isKeyPressed('A')){
		_tx = -_velocityTranslate;
	}

	if(_input->isKeyPressedOnce(GLFW_KEY_KP_ADD)){
		reflectionDepth++;
	}

	if(_input->isKeyPressedOnce(GLFW_KEY_KP_SUBTRACT)){
		reflectionDepth--;
	}
	
	glfwPollEvents();
	if(!_input->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)){
		glfwPollEvents();
		int x = _input->getXPosDiff();
		if(x > 0){
			_ry += _velocityRotate*x;
		}

		if(x < 0){
			_ry += _velocityRotate*x;
		}

		glfwPollEvents();
		int y = _input->getYPosDiff();
		if(y > 0){
			_rx += _velocityRotate*y;
		}

		if(y < 0){
			_rx += _velocityRotate*y;
		}
	} else {
		glfwPollEvents();
		_input->getYPosDiff();
		_input->getXPosDiff();
	}

	glm::mat4 mTransX = glm::translate(cam.getXAxis()*_tx);
	glm::mat4 mTransY = glm::translate(cam.getYAxis()*_ty);
	glm::mat4 mTransZ = glm::translate(cam.getDir()*_tz);

	cam.setTransformRotate( glm::rotate(_ry, cam.getYAxis())*glm::rotate(_rx, cam.getXAxis()) );
	cam.setTransformTranslate(mTransX*mTransY*mTransZ);

	_tx = _ty = _tz = _rx = _ry = _rz = 0;
}