#version 450 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform mat4 cc_mat; // color-correction matrix
uniform vec4 cc_off; // color-correction 5th column

vec3 AdjustBrightness (vec3 color, float value) {
	return color + value;
}

vec3 AdjustContrast (vec3 color, float value) {
  return 0.5 + (1.0 + value) * (color - 0.5);
}

vec3 AdjustExposure (vec3 color, float value) {
  return (1.0 + value) * color;
}

vec3 AdjustSaturation (vec3 color, float value) {
  // https://www.w3.org/TR/WCAG21/#dfn-relative-luminance
  const vec3 luminosityFactor = vec3(0.2126, 0.7152, 0.0722);

  vec3 grayscale = vec3(dot(color, luminosityFactor));
  return mix(grayscale, color, 1.0 + value);
}

void main () {
	//// GAMMA
	//float gamma = 2.2;
	//vec3 diffuseColor = pow(texture(screenTexture, texCoords).rgb, vec3(gamma));

	

	//// COLOR CORRECTION "SHADER CALC"
	//vec3 diffuseColor = texture(screenTexture, texCoords).rgb;
	//diffuseColor = AdjustBrightness (diffuseColor, 0.05f);
	//diffuseColor = AdjustContrast (diffuseColor, 0.2f);
	//// The change of brightness is proportional to the luminosity of the colours.
	//diffuseColor = AdjustExposure (diffuseColor, -0.1f);
	//// We adjust the contrast between channels.
	////  essentially moving between a grayscale variant and the original image.
	//diffuseColor = AdjustSaturation (diffuseColor, -0.5f);
	////
	//vec4 finalColor = vec4 (diffuseColor.x, diffuseColor.y, diffuseColor.z, 1.0f);
	//FragColor = finalColor;

	// COLOR CORRECTION "CPU CALC"
	vec4 texel = texture2D(screenTexture, texCoords);
	FragColor = cc_mat * texel + cc_off;
}