#include "ProcessEngine.h"


void ProcessEngine::setup(PassInfo* sharedPass) {
	setupInputs();
	passData = sharedPass;
	passData->inputStatesRef = &inputStates;
	startOfLastFrame = std::chrono::steady_clock::now();
}