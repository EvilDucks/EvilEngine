#version 330 core
layout (location = 0) in vec3 aPos;

uniform vec3 view;
uniform vec3 projection;
uniform vec3 localSpace;

void main() {
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}