#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 instModel;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 model; // TODO
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * instModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    FragPos = vec3(instModel * vec4(aPos, 1.0));
}