#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in mat4 instModel;
layout (location = 6) in vec3 normal;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 norm;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * instModel * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    FragPos = vec3(instModel * vec4(aPos, 1.0));

    const vec3 normal = vec3(0.0f, 0.0f, 1.0f);
    vec4 temp = normalize(instModel * vec4(normal, 0.0f));
    norm = temp.xyz;
    //norm = vec3(0.0f, -1.0f, 0.0f);

    
}