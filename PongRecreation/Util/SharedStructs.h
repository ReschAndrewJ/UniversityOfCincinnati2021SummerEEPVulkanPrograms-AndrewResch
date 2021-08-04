#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdint>
#include <set>
#include <unordered_map>

namespace inputActions {

	enum inputStates {

		NO_INPUT,
		INPUT_JUST_PRESSED,
		INPUT_HELD,
		INPUT_JUST_RELEASED,

	};

	const std::unordered_map<int, std::string> inputKeys = {
		{GLFW_KEY_W, "P1_UP"},
		{GLFW_KEY_S, "P1_DOWN"},
		{GLFW_KEY_UP, "P2_UP"},
		{GLFW_KEY_DOWN, "P2_DOWN"},
	};

}


namespace enumerables {

	enum bufferSpaceType { vertexData, indexData };

}

namespace SharedStructs
{

	struct pushInfo {
		VkShaderStageFlagBits stageFlags;
		uint32_t offset;
		uint32_t size;
		void* values;
	};

	struct verticesData {
		VkDeviceSize bufferSize;
		void* values;
	};

	struct indicesData {
		VkDeviceSize bufferSize;
		uint32_t indexCount;
		void* values;
	};


	struct bufferSpace {
		void* associatedGraphicsObject;
		enumerables::bufferSpaceType dataUse;
		size_t buffer;
		VkDeviceSize position;
		VkDeviceSize size;
	};

	bool operator<(const bufferSpace& lItem, const bufferSpace& rItem);


};

