#include "InputHandling.h"





void InputHandling::mouseButtonAction(int button, int action) {

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		inputs[inputActions::leftMouseJustPressed] = true;
		inputs[inputActions::leftMouseHeld] = true;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		inputs[inputActions::leftMouseHeld] = false;
		inputs[inputActions::leftMouseJustReleased] = true;
	}

}