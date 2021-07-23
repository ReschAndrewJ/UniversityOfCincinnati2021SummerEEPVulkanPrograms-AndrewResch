#pragma once

#include "glm/glm.hpp"

namespace Shaders {


	struct transformationCompact
	{
		glm::mat4 transformation;
	};

	struct transformationExpand
	{
		glm::mat4 translation;
		glm::mat4 rotation;
		glm::mat4 scale;

		transformationCompact compact() { 
			return transformationCompact{ translation * rotation * scale };
		}
	};


};