#version 330 core

layout (location = 0) in vec3 position; // could be optimized to just vec2 it's supposed to be a plane overall.
layout (location = 1) in vec2 uv;

uniform mat4 projection;

out vec2 fg_uv;

void main() {
	vec2 offset = vec2(0.1 * 41, 0.1 * 10);
	vec2 scale = vec2(200.0, 100.0);

	gl_Position = projection * vec4((position.xy + offset) * scale, 0.0, 1.0);
	//gl_Position = vec4(position.xy, 0.0, 1.0);
	fg_uv = uv;
}