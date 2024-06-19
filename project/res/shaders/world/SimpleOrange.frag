#version 450 core
out vec4 FragColor;

void main() {
	//FragColor = vec4(1.0f, 0.647f, 0.0f, 1.0f);
	float gamma = 2.2;

	vec3 color = vec3(1.0f, 0.647f, 0.0f);
	vec3 diffuseColor = pow(color, vec3(gamma));
	FragColor = vec4(diffuseColor, 1.0f);
} 