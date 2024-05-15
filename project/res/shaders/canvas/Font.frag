#version 330 core
out vec4 FragColor;

uniform sampler2D sampler1;
uniform vec4 color;

in vec2 fg_uv;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1, fg_uv).r);
    FragColor = vec4(color) * sampled;
}  