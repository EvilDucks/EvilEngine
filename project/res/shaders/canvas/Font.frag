#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D sampler1;
uniform vec4 color;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1, TexCoords).r);
    FragColor = vec4(color) * sampled;
}  