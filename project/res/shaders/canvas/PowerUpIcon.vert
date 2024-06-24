#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform mat4 projection;
uniform mat4 model;

out vec2 fg_uv;

void main() {

    gl_Position = projection * model * vec4(position.x, position.y, 0.0, 1.0);

    fg_uv = uv;
}