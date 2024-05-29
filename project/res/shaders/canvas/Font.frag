#version 450 core
out vec4 FragColor;

//uniform sampler2D sampler1;
uniform sampler2DArray sampler1;
uniform int letterMap[384];
uniform vec4 color;

in FG {
    vec2 uv;
    flat int index;
} fg;

void main() {    
    //vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1, fg.uv).r);

    vec3 uvLetter = vec3(fg.uv, letterMap[fg.index]);
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1, uvLetter).r);
    FragColor = vec4(color) * sampled;
}  