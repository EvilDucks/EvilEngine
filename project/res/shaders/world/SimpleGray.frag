#version 450 core
out vec4 FragColor;

void main() {
	//FragColor = vec4(0.33f, 0.33f, 0.33f, 1.0f);
	float gamma = 2.2;

	vec3 color = vec3(0.33f, 0.33f, 0.33f);
	vec3 diffuseColor = pow(color, vec3(gamma));
	FragColor = vec4(diffuseColor, 1.0f);
} 