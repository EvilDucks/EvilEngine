#version 450 core
layout (location = 0) in vec2 vertex;

uniform mat4 projection;
uniform mat4 model;

out vec2 fg_uv;

void main()
{
    gl_Position = projection * model * vec4(vertex.x, vertex.y, 0.0, 1.0);
    fg_uv = vertex.xy;
    fg_uv.y = 1.0 - fg_uv.y;
}  