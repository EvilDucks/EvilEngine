#version 450 core
out vec4 FragColor;

uniform sampler2D sampler1;
uniform float powerUp;

in vec2 fg_uv;

void main() {
    vec4 tex = texture(sampler1, fg_uv);
    if (tex.a == 0)
    {
        discard;
    }
    if (powerUp == 0)
    {
        FragColor = vec4(vec3(tex), 0.0f);
    }
    else
    {
        FragColor = tex;
    }

}