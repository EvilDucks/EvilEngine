#version 450 core
out vec4 FragColor;

uniform sampler2D sampler1;
uniform float buttonState;
  
in vec2 fg_uv;

void main() {
	vec4 tex = texture(sampler1, fg_uv);
	vec3 col = tex.rgb * buttonState;
	if (tex.a == 0)
	{
		discard;
	}
	FragColor = vec4 (col.r, col.g, col.b, tex.a);
	//FragColor = tex;
}