#pragma once

#include <vector>
#include <list>
#include <unordered_map>

#include <mutex>

// class to pass information around using standard classes and data types, no custom classes
class PassInfo
{

public:

	// pointers to collisionShapes to process each processing frame
	std::list<void*> collisionAreas{};

	// mutex to protect graphicsObjectAddVector
	std::mutex gObjectsAddMutex{};
	// mutex to protect graphicsObjectRemoveVector
	std::mutex gObjectsRemoveMutex{};

	// pointers to graphics objects to be created in the graphics engine
	std::vector<void*> graphicsObjectAddVector{};
	// pointers to graphics objects to be destroyed from the graphics engine
	std::vector<void*> graphicsObjectRemoveVector{};
	
	std::mutex eopMutex{};
	bool closeProgram = false;

	std::mutex debugMutex{};

	
	std::unordered_map<std::string, int>* inputStatesRef;


};

