#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instModel;

uniform mat4 view;
uniform mat4 projection;
out vec3 fg_pos;

void main() {
    vec4 temp = instModel * vec4(aPos, 1.0f);
    fg_pos = vec3(temp.x, temp.y, temp.z);
    gl_Position = projection * view * instModel * vec4(aPos, 1.0);
}