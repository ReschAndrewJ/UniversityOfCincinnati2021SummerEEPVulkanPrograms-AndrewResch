#version 450
#extension GL_ARB_seperate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform pushConstants {
	mat4 transformation;
} push;

void main() {
	gl_Position = push.transformation * vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}
