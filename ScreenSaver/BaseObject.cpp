#include "BaseObject.h"

void BaseObject::process(double timeDiff) {};
void BaseObject::onReady() {};

std::vector<BaseObject*> BaseObject::getObjectTree() {
	std::vector<BaseObject*> tree = { this };
	for (BaseObject* obj : children) {
		std::vector<BaseObject*> childTree = obj->getObjectTree();
		tree.insert(tree.end(), childTree.begin(), childTree.end());
	}
	return tree;
}


void BaseObject::addChildObject(BaseObject* childObj) {
	childObj->parentApp_addObjectFunc = parentApp_addObjectFunc;
	childObj->parentApp_removeObjectFunc = parentApp_removeObjectFunc;
	childObj->parentApp_shouldUpdateFunc = parentApp_shouldUpdateFunc;
	childObj->parentApp = parentApp;
	childObj->parentObject = this;
	children.push_back(childObj);
	parentApp_addObjectFunc(childObj, parentApp);
}

void BaseObject::pushToRemoveVector() {
	parentObject->children.erase(std::find(parentObject->children.begin(), parentObject->children.end(), this));
	parentApp_removeObjectFunc(getObjectTree(), parentApp);
}



void BaseObject::onAdd() {
	for (BaseObject* child : children) {
		child->parentApp_addObjectFunc = parentApp_addObjectFunc;
		child->parentApp_removeObjectFunc = parentApp_removeObjectFunc;
		child->parentApp_shouldUpdateFunc = parentApp_shouldUpdateFunc;
		child->parentApp = parentApp;
		child->parentObject = this;
		parentApp_addObjectFunc(child, parentApp);
	}
	onReady();
}

