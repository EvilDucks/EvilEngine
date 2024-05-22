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
	const float specularPower = 5.0f;
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

	const float lightConstant = 1.0f;
	const float lightLinear = 0.1f;
	const float lightQuadratic = 0.1f;

	//vec3 dx = dFdx(fg_pos);
	//vec3 dy = dFdy(fg_pos);
	//vec3 normal = normalize(cross(dx, dy));

	//vec3 baseColor = texture(sampler1, fg_uv).rgb;
	vec3 baseColor = fg_color;
	vec3 coldColor = vec3(0.0f, 0.3f, 0.9f);
	vec3 warmColor = vec3(0.9f, 0.5f, 0.1f);

	vec3 lightDirection = normalize(lightPosition - fg_pos);
	float lightDistance = length(lightPosition - fg_pos);

	// light fading + distance
	float attenuation = 1.0 / (
		lightConstant + lightLinear * 
		lightDistance + lightQuadratic * 
		(lightDistance * lightDistance)
	);

	coldColor = min(coldColor.rgb + (coldDiffuse * baseColor.rgb), 1.0f);
	warmColor = min(warmColor.rgb + (warmDiffuse * baseColor.rgb), 1.0f);

	vec3 color = vec3(mix(coldColor, warmColor, dot(fg_normal, lightDirection) * attenuation));

	vec4 totalLight = vec4(lightAmbient, 1.0) * laIntensity;
	totalLight += CalculateLightColor(lightDiffuse, ldIntensity, fg_pos, lightDirection, fg_normal);

	// Add specular lighting to the color
	FragColor = vec4(color, 1.0f) * totalLight;

	// Simple Texture display.
	//FragColor				= color;
	//FragColor				= texture(sampler1, fg_uv);
	//FragColor				= vec4(fg_color, 1.0f);
}