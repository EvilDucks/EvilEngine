#version 450 core
out vec4 FragColor;
  
in vec3 fg_color;
in vec3 fg_pos;
in vec3 fg_normal;
in vec2 fg_uv;

uniform vec3 lightPosition;

uniform vec3 lightAmbient;
uniform float laIntensity;
uniform vec3 lightDiffuse;
uniform float ldIntensity;

uniform sampler2D sampler1; // diffuseMap

// lightDiffuse, lightIntensity - are light properties
// fg_pos, lightDirection, fg_normal - are vertex/fragment properties
vec4 CalculateLightColor (vec3 lightColor, float lightIntensity, vec3 fragPosition, vec3 fragDirection, vec3 fragNormal) {
	const float specularPower = 1.0f;
	const float reflectance = 0.5f;

	vec4 specularColor = vec4(0, 0, 0, 0);
	
	// Specular Light
	vec3 cameraDirection = normalize(-fragPosition);
	vec3 rDirection = -fragDirection;	// To get fragment direction from light not light direction from light.

	vec3 reflectedLight = normalize(reflect(rDirection, fragNormal));
	float specularFactor = max(dot(cameraDirection, reflectedLight), 0.0);
	specularFactor = pow(specularFactor, specularPower);
	specularColor = lightIntensity * specularFactor * reflectance * vec4(lightColor, 1.0);

	return specularColor;
}

void main() {
	const float coldDiffuse = 0.2;
	const float warmDiffuse = 0.2;

	//vec4 baseColor = texture(sampler1, fg_uv);
	vec4 baseColor = vec4(fg_color, 1.0f);
	vec4 coldColor = vec4(0.0f, 0.3f, 0.9f, 1.0f);
	vec4 warmColor = vec4(0.9f, 0.5f, 0.1f, 1.0f);

	vec3 lightDirection = normalize(lightPosition - fg_pos);

	coldColor = min(coldColor.rgba + coldDiffuse * baseColor.rgba, 1);
	warmColor = min(warmColor.rgba + warmDiffuse * baseColor.rgba, 1);

	vec4 color = mix(coldColor, warmColor, dot(lightDirection, fg_normal));

	vec4 totalLight = vec4(lightAmbient, 1.0) * laIntensity;
	//totalLight += calcDirectionalLight(directionalLight, mvVertexPos, mvVertexNormal);
	//	calcLightColour(light.colour, light.intensity, position, normalize(light.direction), normal);
	totalLight += CalculateLightColor(lightDiffuse, ldIntensity, fg_pos, lightDirection, fg_normal);

	// Add specular lighting to the color
	FragColor = color * totalLight;

	// Simple Texture display.
	//FragColor				= color;
	//FragColor				= texture(sampler1, fg_uv);
	//FragColor				= vec4(fg_color, 1.0f);

	//vec3 camPos             = viewPosition;
	//vec3 lightDir           = normalize(lightPosition - fg_pos);  
	//vec3 viewDir            = normalize(camPos - fg_pos);
	//vec3 norm               = normalize(fg_normal);
	//
	////Get outline
	//float edge_thresh       = 0.0;                                  // Set threshold for edge detection
	//float visiblity         = dot(viewDir, norm);
	//float edge_detection    = (visiblity > edge_thresh) ? 0 : 1; 	// Black color if dot product is smaller than threshold else keep the same colors
	//
	////Gooch Shading
	//vec3 yellow             = vec3(1.0, 1.0, 0.0);  // WARM COLOR
	//vec3 blue               = vec3(0.0, 0.0, 1.0);  // COLD COLOR
	//float Kb                = 0.4;
	//float Ky                = 0.4;
	//float Kdiff             = 1.0; 
	//float alpha             = 0.3;
	//float beta              = 0.6;
	//
	//// Mixing cool and warm colors with the original
	//vec3 Kcool              = Kb * blue + alpha * Kdiff * fg_color;
	//vec3 Kwarm              = Ky * yellow + beta * Kdiff * fg_color;
	//
	//float color_factor      = (1 + dot(norm, lightDir)) / 2;
	//vec3 cool_to_warm       = (color_factor) * Kcool + (1 - color_factor) * Kwarm;
	//
	//// Final color for object
	//vec3 tex_final      	= texture(sampler1, fg_uv).rgb;
	//vec3 final_color		= cool_to_warm;
	//
	//
	//float scale_origin 		= 0.5;
	//float scale				= scale_origin + edge_thresh;
	//float factor			= (visiblity + scale_origin) / scale;		
	//final_color				= factor * final_color;						// teapot
	//
	//FragColor = vec4(final_color, 1.0f);

	// Simple Texture display.
	//FragColor             = texture(sampler1, fg_uv);
	//FragColor 			= vec4(fg_color, 1.0f);
}