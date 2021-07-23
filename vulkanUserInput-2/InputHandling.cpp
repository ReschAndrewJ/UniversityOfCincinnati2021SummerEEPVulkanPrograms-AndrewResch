#include "InputHandling.h"





void InputHandling::mouseButtonAction(int button, int action) {

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			inputs[inputActions::leftMouseJustPressed] = true;
			inputs[inputActions::leftMouseHeld] = true;
		}
		else if (action == GLFW_RELEASE) {
			inputs[inputActions::leftMouseHeld] = false;
			inputs[inputActions::leftMouseJustReleased] = true;
		}
	}

}



void InputHandling::keyAction(int key, int action) {

	if (key == GLFW_KEY_LEFT) {
		if (action == GLFW_PRESS) {
			inputs[inputActions::leftButtonHeld] = true;
		}
		else if (action == GLFW_RELEASE) {
			inputs[inputActions::leftButtonHeld] = false;
		}
	}
	else if (key == GLFW_KEY_RIGHT) {
		if (action == GLFW_PRESS) {
			inputs[inputActions::rightButtonHeld] = true;
		}
		else if (action == GLFW_RELEASE) {
			inputs[inputActions::rightButtonHeld] = false;
		}
	}
	else if (key == GLFW_KEY_UP) {
		if (action == GLFW_PRESS) {
			inputs[inputActions::upButtonHeld] = true;
		}
		else if (action == GLFW_RELEASE) {
			inputs[inputActions::upButtonHeld] = false;
		}
	}
	else if (key == GLFW_KEY_DOWN) {
		if (action == GLFW_PRESS) {
			inputs[inputActions::downButtonHeld] = true;
		} 
		else if (action == GLFW_RELEASE) {
			inputs[inputActions::downButtonHeld] = false;
		}
	}

	

}