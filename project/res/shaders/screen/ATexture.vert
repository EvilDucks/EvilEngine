#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord; // not needed

uniform float x_dir;
uniform float y_dir;

out vec2 TexCoord;

void main() {
    
    //gl_Position = vec4(aPos.x + x_dir - 0.5, aPos.y + y_dir - 0.5, 0.0, 1.0);
    //TexCoord = vec2(aPos.x, aPos.y);

    //gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos.x + x_dir, aPos.y + y_dir, 0.0, 1.0);
}