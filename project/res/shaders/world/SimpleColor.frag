#version 330 core
out vec4 FragColor;

uniform vec4 color;
in vec3 fg_pos;
uniform vec3 camPos = vec3(0.0, 0.0, 0.0);

void main() {
// fog
    float distance = length(camPos - fg_pos)*0.01;
    float density = 1.2;
    float fogFactor = exp(-density * density * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 fog = vec4(0.5, 0.5, 0.5, 1.0);

	FragColor = mix(fog, color, fogFactor);
} 