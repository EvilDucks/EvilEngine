#version 450 core

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

uniform mat4 projection;
uniform mat4 model;
//uniform vec2 size;

out vec2 fg_uv;

void main() {

    vec2 size   = vec2(100.0, 200.0);

    gl_Position = projection * model * vec4(position.x, position.y, 0.0, 1.0);

    fg_uv = uv;
}