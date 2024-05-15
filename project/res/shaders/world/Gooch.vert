#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in mat4 instanceModel;
layout (location = 3) in vec3 normal;

out vec3 fg_color;
out vec3 fg_pos;
out vec3 fg_normal;
out vec2 fg_uv;

uniform mat4 view;
uniform mat4 projection;

void main() {
	//gl_Position = projection * view * instanceModel * vec4(position.x *0.245, position.y *0.245, position.z *0.245, 1.0);
	gl_Position = projection * view * instanceModel * vec4(position, 1.0);

	vec4 temp = view * vec4(position, 1.0f); 
	fg_pos = vec3(temp.x, temp.y, temp.z);
    
	temp = transpose(inverse(view)) * vec4(normal, 1.0f);
	fg_normal = vec3(temp.x, temp.y, temp.z);
   
    fg_color = vec3(1.0, 1.0, 1.0);
	fg_uv = uv;
}