#include "ProcessEngine.h"


void ProcessEngine::setupInputs() {
	inputStates = std::unordered_map<std::string, int>{};
	for (auto it = inputActions::inputKeys.begin(); it != inputActions::inputKeys.end(); ++it) {
		inputStates[(*it).second] = inputActions::NO_INPUT;
	}
	statesFromCallback = std::unordered_map<std::string, inputActions::inputStates>{};
	for (auto it = inputActions::inputKeys.begin(); it != inputActions::inputKeys.end(); ++it) {
		statesFromCallback[(*it).second] = inputActions::NO_INPUT;
	}
}

void ProcessEngine::advanceInputs() {
	std::lock_guard<std::mutex> inputGuard(inputCallbackMutex);

	for (auto it = inputActions::inputKeys.begin(); it != inputActions::inputKeys.end(); ++it) {
		switch (inputStates[(*it).second])
		{
		case inputActions::INPUT_JUST_PRESSED:
			inputStates[(*it).second] = inputActions::INPUT_HELD;
			break;
		case inputActions::INPUT_JUST_RELEASED:
			inputStates[(*it).second] = inputActions::NO_INPUT;
			break;
		default:
			break;
		}

		switch (statesFromCallback[(*it).second])
		{
		case inputActions::INPUT_JUST_PRESSED:
			inputStates[(*it).second] = inputActions::INPUT_JUST_PRESSED;
			break;
		case (int)inputActions::INPUT_JUST_RELEASED:
			inputStates[(*it).second] = inputActions::INPUT_JUST_RELEASED;
			break;
		default:
			break;
		}
		statesFromCallback[(*it).second] = inputActions::NO_INPUT;
	}
}


void ProcessEngine::keyCallback(int key, int action) {
	std::lock_guard<std::mutex> inputGuard(inputCallbackMutex);
	try {
		switch (action)
		{
		case GLFW_PRESS:
			statesFromCallback.at(inputActions::inputKeys.at(key)) = inputActions::INPUT_JUST_PRESSED;
			break;
		case GLFW_RELEASE:
			statesFromCallback.at(inputActions::inputKeys.at(key)) = inputActions::INPUT_JUST_RELEASED;
			break;
		default:
			break;
		}
	}
	catch (std::out_of_range oor) {}
	

}