#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in mat4 instModel;
layout (location = 5) in vec2 uv;
layout (location = 6) in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
out vec3 fg_pos;
out vec3 fg_normal;
out vec2 fg_uv;

void main() {
	fg_uv = uv;
	fg_normal = normal;

    vec4 temp = instModel * vec4(aPos, 1.0f);
    fg_pos = vec3(temp.x, temp.y, temp.z);
    gl_Position = projection * view * instModel * vec4(aPos, 1.0);
}