#version 450 core
out vec4 FragColor;

uniform sampler2D sampler1;
uniform vec4 color;
uniform float visibility;

in vec2 fg_uv;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1, fg_uv).r);
    if (sampled.a == 0 || visibility == 0)
    {
        discard;
    }
    FragColor = vec4(color) * sampled;
}  