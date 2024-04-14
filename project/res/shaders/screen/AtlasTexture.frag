#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D sampler1;
uniform float uv_x;
uniform float uv_y;
uniform float nx_frames;
uniform float ny_frames;

void main() {
    float x = 1.0 / nx_frames;
    float y = 1.0 / ny_frames;
    FragColor = texture(sampler1, vec2(TexCoord.x * x, TexCoord.y * y) + vec2(x * uv_x, y * uv_y));
}