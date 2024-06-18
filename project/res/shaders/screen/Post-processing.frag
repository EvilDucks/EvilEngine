#version 430 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform vec2 windowDimensions;
uniform vec2 motionBlur;

const float offset_x = 1.0f / 800.0f;
const float offset_y = 1.0f / 800.0f;

vec2 offsets[9] = vec2[]
(
    vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
    vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
    vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y)
);

float kernel[9] = float[]
(
    1,  1, 1,
    1, -8, 1,
    1,  1, 1
);

void main()
{
//    vec3 color = vec3(0.0f);
//    for(int i = 0; i < 9; i++)
//        color += vec3(texture(screenTexture, texCoords.st + offsets[i])) * kernel[i];

    vec2 texCoord = texCoords;
    vec2 velocity = motionBlur;
    int g_numSamples = 500;
    
    // Motion blur
    vec4 color = texture(screenTexture, texCoord);
    for (int i = 0; i < g_numSamples; i++) {
        texCoord += velocity;
        // Sample the color buffer along the velocity vector.
        vec4 currentColor = texture(screenTexture, texCoord);
        // Add the current color to our color sum.
        color += currentColor;
    } // Average all of the samples to get the final blur color.
    vec4 finalColor = color / g_numSamples;
    FragColor = finalColor;
}