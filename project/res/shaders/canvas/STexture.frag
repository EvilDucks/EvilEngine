#version 330 core
out vec4 FragColor;

uniform sampler2D sampler1;
//uniform float buttonState;
uniform vec4 color;
  
in vec2 fg_uv;

void main() {
    float sample = 1.0f;
    FragColor = vec4(color.x, 0.5f, 0.2f, 1.0f);
    //FragColor = texture(sampler1, fg_uv);
}