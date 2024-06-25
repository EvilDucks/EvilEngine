#version 450 core
out vec4 FragColor;

uniform sampler2D sampler1;
uniform float powerUpPercentage;

in vec2 fg_uv;

void main() {
    vec4 tex = texture(sampler1, fg_uv);
//    if (charge < 0)
//    {
//        charge = 0;
//    }
    if (fg_uv.x >= powerUpPercentage)
    {
        tex.a *= 0.25f;
    }
    if (tex.a == 0)
    {
        discard;
    }
    FragColor = tex;
}