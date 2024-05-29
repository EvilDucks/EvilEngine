#version 450 core
out vec4 FragColor;

//uniform sampler2D sampler1;
uniform sampler2DArray sampler1a;
uniform int letterMap[384];
uniform vec4 color;

struct FG {
    vec2 uv;
    int index;
}; 

in FG fg;

void main() {    
	//FragColor = vec4(color);

	//vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1, fg.uv).r);

	//vec3 uvLetter = vec3(fg.uv, 0);
	//vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1a, uvLetter).r);

	// final
	//vec3 uvLetter = vec3(fg.uv, letterMap[fg.index]);
	//vec4 sampled = vec4(1.0, 1.0, 1.0, texture(sampler1a, uvLetter).r);
	//FragColor = vec4(color) * sampled;
}  