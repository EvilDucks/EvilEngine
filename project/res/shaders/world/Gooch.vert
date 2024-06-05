#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in mat4 instanceModel;
layout (location = 5) in vec2 uv;
layout (location = 6) in vec3 normal;

out vec3 fg_color;
out vec3 fg_pos;
out vec3 fg_normal;
out vec2 fg_uv;

uniform mat4 view;
uniform mat4 projection;

void main() {
	vec4 temp = instanceModel * vec4(position, 1.0f); 
	gl_Position = projection * view * temp;

	fg_pos = vec3(temp.x, temp.y, temp.z);
    
	temp = normalize(instanceModel * vec4(normal, 0.0f));
	fg_normal = vec3(temp.x, temp.y, temp.z);
   
	// Imagine this is passed through a layout param. &color.
	fg_color = vec3(1.0, 1.0, 1.0);
	fg_uv = uv;
}