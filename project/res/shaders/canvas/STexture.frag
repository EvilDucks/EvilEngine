#version 330 core
out vec4 FragColor;

uniform sampler2D sampler1;
  
in vec2 fg_uv;

void main() {
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = texture(sampler1, fg_uv);
}