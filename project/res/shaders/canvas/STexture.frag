#version 330 core
out vec4 FragColor;

uniform sampler2D sampler1;
uniform float buttonState;
  
in vec2 fg_uv;

void main() {
	vec4 tex = texture(sampler1, fg_uv);
	vec3 col = tex.rgb * buttonState;
	FragColor = vec4 (col.r, col.g, col.b, tex.a);
}