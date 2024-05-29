#version 330 core
out vec4 FragColor;
  
in vec3 fg_color;
in vec3 fg_pos;
in vec3 fg_normal;
in vec2 fg_uv;

uniform mat4 view;
uniform vec3 viewPosition;
uniform vec3 lightPosition;
uniform vec3 lightDiffuse;
uniform sampler2D sampler1; // diffuseMap

void main() {
    vec3 camPos             = viewPosition;
	vec3 lightDir           = normalize(lightPosition - fg_pos);  
	vec3 viewDir            = normalize(camPos - fg_pos);
	vec3 norm               = normalize(fg_normal);

	//Get outline
	float edge_thresh       = 0.0;                                  // Set threshold for edge detection
	float visiblity         = dot(viewDir, norm);
	float edge_detection    = (visiblity > edge_thresh) ? 0 : 1; 	// Black color if dot product is smaller than threshold else keep the same colors

	//Gooch Shading
	vec3 yellow             = vec3(1.0, 1.0, 0.0);  // WARM COLOR
	vec3 blue               = vec3(0.0, 0.0, 1.0);  // COLD COLOR
	float Kb                = 0.4;
	float Ky                = 0.4;
	float Kdiff             = 1.0; 
	float alpha             = 0.3;
	float beta              = 0.6;

    // Mixing cool and warm colors with the original
	vec3 Kcool              = Kb * blue + alpha * Kdiff * fg_color;
	vec3 Kwarm              = Ky * yellow + beta * Kdiff * fg_color;

	float color_factor      = (1 + dot(norm, lightDir)) / 2;
	vec3 cool_to_warm       = (color_factor) * Kcool + (1 - color_factor) * Kwarm;

	// Final color for object
	vec3 tex_final      	= texture(sampler1, fg_uv).rgb;
	vec3 final_color		= cool_to_warm;
    

	float scale_origin 		= 0.5;
	float scale				= scale_origin + edge_thresh;
	float factor			= (visiblity + scale_origin) / scale;		
	final_color				= factor * final_color;						// teapot

	FragColor = vec4(final_color, 1.0f);

    // Simple Texture display.
    //FragColor             = texture(sampler1, fg_uv);
}