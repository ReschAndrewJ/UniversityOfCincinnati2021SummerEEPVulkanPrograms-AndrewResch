#include "GraphicsEngine.h"


void GraphicsEngine::framebufferResizedCallback(GLFWwindow* window, int width, int height) {
	auto engine = reinterpret_cast<GraphicsEngine*>(glfwGetWindowUserPointer(window));
	engine->framebufferResized = true;
}


