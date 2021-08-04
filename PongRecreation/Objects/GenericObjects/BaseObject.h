#pragma once

#include "../GraphicsObject.h"
#include "../../ApplicationEngine/PassInfo.h"

class BaseObject
{

	friend class Root;

	BaseObject* parentObject;
	std::vector<BaseObject*> childObjects;

	GraphicsObject* gObject;
	void* parentApp;
	


protected:

	PassInfo* passData;

	void createGraphicsObject(std::string vertShaderFilepath, std::string fragShaderFilepath);

	void sendPushConstants(const std::vector<SharedStructs::pushInfo>& constants);

	int getInputState(std::string inputKey);


public:

	std::vector<BaseObject*> getDescendants();
	void addChildObject(BaseObject* object);
	void free();


	static SharedStructs::verticesData sendVertexDataCallback(void* object);
	static SharedStructs::indicesData sendIndicesDataCallback(void* object);

	static void sendVertexBindingDescriptionCallback(void* object, VkVertexInputBindingDescription& bindingDesc);
	static void sendVertexAttributeDescriptionsCallback(void* object, std::vector<VkVertexInputAttributeDescription>& attrDescs);

	static void sendDescriptorSetLayoutBindingsCallback(void* object, std::vector<VkDescriptorSetLayoutBinding>& descSetLayoutBindings);
	static void sendPushConstantsRangesCallback(void* object, std::vector<VkPushConstantRange>& pushConstRanges);

	virtual void processFrame(double timeDiff);

private:


	virtual SharedStructs::verticesData sendVertexData();
	virtual SharedStructs::indicesData sendIndicesData();

	virtual void sendVertexBindingDescription(VkVertexInputBindingDescription& bindingDesc);
	virtual void sendVertexAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attrDescs);

	virtual void sendDescriptorSetLayoutBindings(std::vector<VkDescriptorSetLayoutBinding>& descSetLayoutBindings);
	virtual void sendPushConstantsRanges(std::vector<VkPushConstantRange>& pushConstRanges);

	virtual void preReady();
	virtual void Ready();

};

