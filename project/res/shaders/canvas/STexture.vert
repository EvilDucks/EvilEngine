#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform mat4 projection;
uniform mat4 model;
//uniform vec2 size;

out vec2 fg_uv;

void main() {

	vec2 size   = vec2(200.0, 100.0);

	//mat4 model = mat4(1.0);
	//model[3][0] = 700.0; // x position
	//model[3][1] = 50.0;  // y position

	gl_Position = projection * model * vec4(position.x, position.y, 0.0, 1.0);

	//gl_Position = projection * vec4(
	//	(position.x * size.x) + offset.x,
	//	(position.y * size.y) + offset.y,
	//	0.0, 1.0
	//);

	fg_uv = uv;
}