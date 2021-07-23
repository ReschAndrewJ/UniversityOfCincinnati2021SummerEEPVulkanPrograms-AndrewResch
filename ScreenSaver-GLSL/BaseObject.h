#pragma once
#include "GraphicalObject.h"

class BaseObject : public GraphicalObject
{

public:

	// Public Members

	std::vector<BaseObject*> children;
	BaseObject* parentObject;

	void* parentApp;
	void (*parentApp_addObjectFunc)(BaseObject*, void*);
	void (*parentApp_removeObjectFunc)(std::vector<BaseObject*>, void*);
	void (*parentApp_updateObjectFunc)(BaseObject*, void*); // call changes to the vertex and index buffers

	// Public Functions

	virtual void process(double timeDiff);

	std::vector<BaseObject*> getObjectTree();

	void addChildObject(BaseObject*);
	void pushToRemoveVector();

	void onAdd();

protected:

	virtual void onReady();

};

