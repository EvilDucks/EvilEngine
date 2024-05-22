#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform mat4 projection;

out vec2 fg_uv;

void main() {

	vec2 offset = vec2(700.0, 50.0);
	vec2 size   = vec2(200.0, 100.0);

	gl_Position = projection * vec4(
		(position.x * size.x) + offset.x,
		(position.y * size.y) + offset.y,
		0.0, 1.0
	);

	fg_uv = uv;
}