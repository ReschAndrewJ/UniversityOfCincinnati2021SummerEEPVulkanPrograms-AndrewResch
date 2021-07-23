#pragma once

#include "GLFW/glfw3.h"

#include <vector>


namespace inputActions {
	enum InputsIndeces {
		leftMouseJustPressed, leftMouseHeld, leftMouseJustReleased,
		INPUTS_SIZE
	};
}


class InputHandling
{

public:

	std::vector<bool> inputs = std::vector((inputActions::INPUTS_SIZE), false);

	void mouseButtonAction(int, int);


};

