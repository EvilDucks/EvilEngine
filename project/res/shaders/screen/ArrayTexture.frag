#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2DArray sampler1a;
uniform int tile;

void main() {
    FragColor = texture(sampler1a, vec3(texCoord.x, texCoord.y, tile));
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}