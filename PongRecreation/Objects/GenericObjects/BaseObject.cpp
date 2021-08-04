#include "BaseObject.h"


void BaseObject::createGraphicsObject(std::string vertShaderFilepath, std::string fragShaderFilepath) {
	std::lock_guard<std::mutex> addGuard(passData->gObjectsAddMutex);
	gObject = new GraphicsObject();
	gObject->ownerObject = (void*)this;
	gObject->setVertexShaderFilepath(vertShaderFilepath);
	gObject->setFragmentShaderFilePath(fragShaderFilepath);

	gObject->getVertexData = sendVertexDataCallback;
	gObject->getIndicesData = sendIndicesDataCallback;
	gObject->getVertexBindingDescription = sendVertexBindingDescriptionCallback;
	gObject->getVertexAttributeDescriptions = sendVertexAttributeDescriptionsCallback;
	gObject->getDescriptorSetLayoutBindings = sendDescriptorSetLayoutBindingsCallback;
	gObject->getPushConstantsRanges = sendPushConstantsRangesCallback;

	passData->graphicsObjectAddVector.push_back(gObject);
}


void BaseObject::free() {
	std::lock_guard<std::mutex> removeGuard(passData->gObjectsRemoveMutex);
	if (gObject != nullptr) { passData->graphicsObjectRemoveVector.push_back(gObject); }
	for (BaseObject* descendant : getDescendants()) {
		passData->graphicsObjectRemoveVector.push_back(descendant->gObject);
		delete descendant;
	}
	if (parentObject != nullptr) {
		parentObject->childObjects.erase(std::find(parentObject->childObjects.begin(),
			parentObject->childObjects.end(), this));
	}
	delete this;
}


std::vector<BaseObject*> BaseObject::getDescendants() {
	std::vector<BaseObject*> descendants{};
	for (BaseObject* child : childObjects) {
		std::vector <BaseObject*> childsDescendants = child->getDescendants();
		if (childsDescendants.size() > 0) {
			descendants.insert(descendants.end(), childsDescendants.begin(), childsDescendants.end());
		}
		descendants.push_back(child);
	}
	return descendants;
}


void BaseObject::addChildObject(BaseObject* object) {
	object->parentApp = parentApp;
	object->parentObject = this;
	object->passData = passData;

	childObjects.push_back(object);
	object->preReady();
	object->Ready();
}


int BaseObject::getInputState(std::string inputKey) {
	try {
		return passData->inputStatesRef->at(inputKey);
	}
	catch (std::out_of_range oop) {
		std::cout << "Input: " << inputKey << " is not an implemented key" << std::endl;
		return -1;
	}
}


// ------------------------------------------------

void BaseObject::preReady() {
	createGraphicsObject("Shaders/Spv/BaseVert.spv", "Shaders/Spv/BaseFrag.Spv");
}

void BaseObject::Ready() {}

void BaseObject::processFrame(double timeDiff) {}

// ------------------------------------------------

SharedStructs::verticesData BaseObject::sendVertexDataCallback(void* object) {
	return ((BaseObject*)object)->sendVertexData();
}
SharedStructs::verticesData BaseObject::sendVertexData() {
	SharedStructs::verticesData data{};
	data.bufferSize = 0;
	data.values = nullptr;
	return data;
}


SharedStructs::indicesData BaseObject::sendIndicesDataCallback(void* object) {
	return ((BaseObject*)object)->sendIndicesData();
}
SharedStructs::indicesData BaseObject::sendIndicesData() {
	SharedStructs::indicesData data{};
	data.bufferSize = 0;
	data.indexCount = 0;
	data.values = nullptr;
	return data;
}


void BaseObject::sendVertexBindingDescriptionCallback(void* object, VkVertexInputBindingDescription& bindingDescs) {
	((BaseObject*)object)->sendVertexBindingDescription(bindingDescs);
}
void BaseObject::sendVertexBindingDescription(VkVertexInputBindingDescription& description) {
	description.binding = 0;
	description.stride = 0;
	description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}


void BaseObject::sendVertexAttributeDescriptionsCallback(void* object, std::vector<VkVertexInputAttributeDescription>& attrDescs) {
	((BaseObject*)object)->sendVertexAttributeDescriptions(attrDescs);
}
void BaseObject::sendVertexAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attrDescs) {
	attrDescs = std::vector<VkVertexInputAttributeDescription>{};
}


void BaseObject::sendDescriptorSetLayoutBindingsCallback(void* object, std::vector<VkDescriptorSetLayoutBinding>& descSetLayoutBindings) {
	((BaseObject*)object)->sendDescriptorSetLayoutBindings(descSetLayoutBindings);
}
void BaseObject::sendDescriptorSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& descSetLayoutBindings) {
	descSetLayoutBindings = std::vector<VkDescriptorSetLayoutBinding>{};
}


void BaseObject::sendPushConstantsRangesCallback(void* object, std::vector<VkPushConstantRange>& pushConstRanges) {
	((BaseObject*)object)->sendPushConstantsRanges(pushConstRanges);
}
void BaseObject::sendPushConstantsRanges(std::vector<VkPushConstantRange>& pushConstRanges) {
	pushConstRanges = std::vector<VkPushConstantRange>{};
}

void BaseObject::sendPushConstants(const std::vector<SharedStructs::pushInfo>& constants) {
	gObject->setPushConstants(constants);
}