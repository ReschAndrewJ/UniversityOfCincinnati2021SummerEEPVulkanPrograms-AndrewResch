#include "GraphicsEngine.h"


void GraphicsEngine::createDestroyGraphicsObjects() {
	std::lock_guard<std::mutex> addlock(passData->gObjectsAddMutex);
	std::lock_guard<std::mutex> removelock(passData->gObjectsRemoveMutex);
	
	if (passData->graphicsObjectRemoveVector.size() > 0) {

		std::vector<GraphicsObject*> gObjsToDestroy{};
		for (void* obj : passData->graphicsObjectRemoveVector) {
			gObjsToDestroy.push_back((GraphicsObject*)obj);
			drawObjects.erase(std::find(drawObjects.begin(), drawObjects.end(),
				(GraphicsObject*)obj));
		}

		passData->graphicsObjectRemoveVector.clear();
		destroyGraphicsObjects(gObjsToDestroy);
	}

	if (passData->graphicsObjectAddVector.size() > 0) {

		std::vector<GraphicsObject*> gObjsToCreate{};
		for (void* obj : passData->graphicsObjectAddVector) {
			gObjsToCreate.push_back((GraphicsObject*)obj);
			drawObjects.push_back((GraphicsObject*)obj);
		}
		passData->graphicsObjectAddVector.clear();
		createGraphicsObjects(gObjsToCreate);

		// refills graphicsObjectAddVector with recycles
		passData->graphicsObjectAddVector.resize(gObjsToCreate.size());
		std::copy(gObjsToCreate.begin(), gObjsToCreate.end(),
			passData->graphicsObjectAddVector.begin());
	}

}


void GraphicsEngine::createGraphicsObjects(std::vector<GraphicsObject*>& newObjects) {
	std::vector<GraphicsObject*> recycles;
	std::vector<GraphicsObject*> passThroughs;

	for (GraphicsObject* obj : newObjects) {
		SharedStructs::verticesData vertices = obj->getVertexData(obj->ownerObject);
		SharedStructs::indicesData indices = obj->getIndicesData(obj->ownerObject);
		if (vertices.bufferSize != 0 && indices.indexCount != 0) {
			if (stageAllocateObject(obj)) { passThroughs.push_back(obj); }
			else { recycles.push_back(obj); }
		}
		obj->createDescriptorSetLayout(logicalDevice);
	}

	newObjects.clear();
	newObjects = recycles;

	flushStagingBuffer();

	GraphicsObject::createGraphicsPipelines(logicalDevice, renderPass,
		swapChainExtent, passThroughs);
}


bool GraphicsEngine::stageAllocateObject(GraphicsObject* object) {
	SharedStructs::verticesData vertInfo = object->getVertexData(object->ownerObject);
	VkDeviceSize vertBeginRange = 0;
	VkDeviceSize vertEndRange = vertInfo.bufferSize - 1; // minus 1 for zero indexing
	bool vertStagingSpaceFound = false;
	
	if (stagingBufferInfo.empty()) {
		if (vertInfo.bufferSize > bufferAllocationSize) {
			throw std::runtime_error("vertex size greater than maximum allocation size");
		}
		vertStagingSpaceFound = true;
	}
	for (const SharedStructs::bufferSpace& space : stagingBufferInfo) {
		if (vertEndRange < space.position) {
			vertStagingSpaceFound = true;
			break;
		}
		else {
			vertBeginRange = space.position + space.size;
			vertEndRange = vertBeginRange + vertInfo.bufferSize - 1;
		}
	}
	if (vertEndRange < bufferAllocationSize) {
		vertStagingSpaceFound = true;
	}

	SharedStructs::indicesData indexInfo = object->getIndicesData(object->ownerObject);
	VkDeviceSize indexBeginRange = 0;
	VkDeviceSize indexEndRange = indexInfo.bufferSize - 1;
	bool indexStagingSpaceFound = false;
	
	if (stagingBufferInfo.empty()) {
		if (indexInfo.bufferSize + vertInfo.bufferSize > bufferAllocationSize) {
			throw (std::runtime_error("cannot fit both index and vertex into staging buffer maximum allocation size"));
		}
		indexBeginRange = vertInfo.bufferSize;
		indexStagingSpaceFound = true;
	}
	for (const SharedStructs::bufferSpace& space : stagingBufferInfo) {
		if (indexBeginRange == vertBeginRange) {
			indexBeginRange = vertEndRange + 1;
			indexEndRange = indexBeginRange + indexInfo.bufferSize - 1;
		}
		if (indexEndRange < space.position) {
			indexStagingSpaceFound = true;
			break;
		}
		else {
			indexBeginRange = space.position + space.size;
			indexEndRange = indexBeginRange + indexInfo.bufferSize - 1;
		}
	}
	if (!indexStagingSpaceFound) {
		if (indexBeginRange == vertBeginRange) {
			indexBeginRange = vertEndRange + 1;
			indexEndRange = indexBeginRange + indexInfo.bufferSize - 1;
		}
		if (indexEndRange < bufferAllocationSize) {
			indexStagingSpaceFound = true;
		}
	}

	if (vertStagingSpaceFound && indexStagingSpaceFound) {
		SharedStructs::bufferSpace vertexSpace;
		vertexSpace.associatedGraphicsObject = object;
		vertexSpace.dataUse = enumerables::bufferSpaceType::vertexData;
		vertexSpace.buffer = -1;
		vertexSpace.position = vertBeginRange;
		vertexSpace.size = vertInfo.bufferSize;

		SharedStructs::bufferSpace indexSpace;
		indexSpace.associatedGraphicsObject = object;
		indexSpace.dataUse = enumerables::bufferSpaceType::indexData;
		indexSpace.buffer = -1;
		indexSpace.position = indexBeginRange;
		indexSpace.size = indexInfo.bufferSize;

		stagingBufferInfo.insert(vertexSpace);
		stagingBufferInfo.insert(indexSpace);

		void* vertexStagingLocation;
		vkMapMemory(logicalDevice, stagingBufferMemory, vertBeginRange,
			vertInfo.bufferSize, 0, &vertexStagingLocation);
		memcpy(vertexStagingLocation, vertInfo.values, (size_t)vertInfo.bufferSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);
		
		void* indexStagingLocation;
		vkMapMemory(logicalDevice, stagingBufferMemory, indexBeginRange,
			indexInfo.bufferSize, 0, &indexStagingLocation);
		memcpy(indexStagingLocation, indexInfo.values, (size_t)indexInfo.bufferSize);
		vkUnmapMemory(logicalDevice, stagingBufferMemory);

		return true;
	}

	return false;
}


void GraphicsEngine::flushStagingBuffer() {

	std::vector<VkBufferCopy> copyRegions;
	std::vector<SharedStructs::bufferSpace> copyBufferInfos;

	for (SharedStructs::bufferSpace newBufferSpaceInfo : stagingBufferInfo) {
		// info coming from staging buffer starts with bufferindex -1
		VkBufferCopy copyRegion{};
		copyRegion.size = newBufferSpaceInfo.size;
		copyRegion.srcOffset = newBufferSpaceInfo.position;
		bool spaceFound = false;
		for (auto& bufferInfo : objectsBuffersInfos) {
			newBufferSpaceInfo.buffer++;
			VkDeviceSize rangeStart = 0;
			VkDeviceSize rangeEnd = newBufferSpaceInfo.size - 1;
			for (const SharedStructs::bufferSpace& filledSpace : bufferInfo) {
				if (rangeEnd < filledSpace.position) {
					spaceFound = true;
					break;
				}
				rangeStart = filledSpace.position + filledSpace.size;
				rangeEnd = rangeStart + newBufferSpaceInfo.size - 1;
			}
			if (!spaceFound && rangeEnd < bufferAllocationSize) {
				spaceFound = true;
			}

			if (spaceFound) {
				newBufferSpaceInfo.position = rangeStart;
				break;
			}
		}
		if (!spaceFound) {
			createObjectsBuffer();
			newBufferSpaceInfo.buffer++;
			newBufferSpaceInfo.position = 0;
		}

		copyBufferInfos.push_back(newBufferSpaceInfo);
		objectsBuffersInfos[newBufferSpaceInfo.buffer].insert(newBufferSpaceInfo);

		if (newBufferSpaceInfo.dataUse == enumerables::vertexData) {
			((GraphicsObject*)newBufferSpaceInfo.associatedGraphicsObject)->vertexMemoryInfo = newBufferSpaceInfo;
		}
		else if (newBufferSpaceInfo.dataUse == enumerables::indexData) {
			((GraphicsObject*)newBufferSpaceInfo.associatedGraphicsObject)->indexMemoryInfo = newBufferSpaceInfo;
		}
		copyRegion.dstOffset = newBufferSpaceInfo.position;
		copyRegions.push_back(copyRegion);
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	if (helperCommandPool != nullptr) { allocInfo.commandPool = helperCommandPool; }
	else { allocInfo.commandPool = drawCommandPool; }
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	for (size_t i = 0; i < copyBufferInfos.size(); i++) {
		vkCmdCopyBuffer(commandBuffer, stagingBuffer, objectsBuffers[copyBufferInfos[i].buffer],
			1, &copyRegions[i]);
	}
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (helperQueue != nullptr) {
		vkQueueSubmit(helperQueue, 1, &submitInfo, VK_NULL_HANDLE);

		vkQueueWaitIdle(helperQueue);
		vkFreeCommandBuffers(logicalDevice, helperCommandPool, 1, &commandBuffer);
	}
	else {
		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

		vkQueueWaitIdle(graphicsQueue);
		vkFreeCommandBuffers(logicalDevice, drawCommandPool, 1, &commandBuffer);
	}
	stagingBufferInfo.clear();
}



// ----------------------------------------------------


void GraphicsEngine::destroyGraphicsObjects(std::vector<GraphicsObject*>& objectsToDestroy) {
	for (GraphicsObject* obj : objectsToDestroy) {
		if (obj->vertexMemoryInfo.size != 0
			&& obj->indexMemoryInfo.size != 0) {
			std::cout << "erasing memory infos" << std::endl;
			std::cout << "erasing vertex info" << std::endl;
			objectsBuffersInfos[obj->vertexMemoryInfo.buffer].erase(obj->vertexMemoryInfo);
			
			if (objectsBuffersInfos[obj->vertexMemoryInfo.buffer].empty()) {
				std::cout << "vertex's memory buffer empty, destroying buffer" << std::endl;
				destroyObjectsBuffer(obj->vertexMemoryInfo.buffer);
			}
			std::cout << "erasing index info" << std::endl;
			objectsBuffersInfos[obj->indexMemoryInfo.buffer].erase(obj->indexMemoryInfo);
			if (objectsBuffersInfos[obj->indexMemoryInfo.buffer].empty()) {
				std::cout << "index's memory buffer empty, destroying buffer" << std::endl;
				destroyObjectsBuffer(obj->indexMemoryInfo.buffer);
			}
		}
		std::cout << "destroying pipeline" << std::endl;
		vkDestroyPipeline(logicalDevice, obj->graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(logicalDevice, obj->graphicsPipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(logicalDevice, obj->descriptorSetLayout, nullptr);
		std::cout << "deleting obj" << std::endl;
		delete obj;
	}
}

