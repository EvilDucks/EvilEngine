#version 450 core
out vec4 FragColor;

uniform sampler2D sampler1;


in vec2 fg_uv;

void main() {
    vec4 tex = texture(sampler1, fg_uv);
    FragColor = tex;
}