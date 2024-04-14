#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord; // not needed

uniform float x_dir;
uniform float y_dir;

out vec2 TexCoord;

void main() {
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos.x + x_dir, aPos.y + y_dir, 0.0, 1.0);
}