#version 450 core
layout (location = 0) in vec2 vertex;

uniform mat4 projection;
uniform mat4 models[384];

out FG {
    vec2 uv;
    flat int index;
} fg;


void main() {
    //gl_Position = projection * model * vec4(vertex.x, vertex.y, 0.0, 1.0);

    gl_Position = projection * models[gl_InstanceID] * vec4(vertex.x, vertex.y, 0.0, 1.0);
    
    fg.index = gl_InstanceID;
    fg.uv = vertex;
    fg.uv.y = 1.0 - fg.uv.y;
}  