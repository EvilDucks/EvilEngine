#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 instModel;
layout (location = 3) in vec3 normal;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main() {
    //vec3 sample = vec3(5.0f, 5.0f, 5.0f);
    //gl_Position = projection * view * instModel * vec4(aPos + sample, 1.0);
    gl_Position = projection * view * instModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}