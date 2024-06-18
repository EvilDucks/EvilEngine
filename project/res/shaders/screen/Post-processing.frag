#version 430 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform vec2 windowDimensions;
uniform vec2 motionBlur;
uniform mat4 view1;
uniform mat4 projection1;
uniform mat4 view2;
uniform mat4 projection2;
uniform mat4 previousView1;
uniform mat4 previousProjection1;
uniform mat4 previousView2;
uniform mat4 previousProjection2;

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
    int viewport = 0;
    vec2 texCoord = texCoords;
    mat4 view = view1;
    mat4 projection = projection1;
    mat4 previousView = previousView1;
    mat4 previousProjection = previousProjection1;

    // Determining to which viewport does the fragment belong
    if (gl_FragCoord.x > windowDimensions.x/2)
    {
        viewport = 1;
        view = view2;
        projection = projection2;
        previousView = previousView2;
        previousProjection = previousProjection2;
    }

    // Get the depth buffer value at this pixel.
    float zOverW = gl_FragDepth;
    // H is the viewport position at this pixel in the range -1 to 1.
    vec4 H = vec4(texCoord.x * 2 - 1, (1 - texCoord.y) * 2 - 1, zOverW, 1);
    // Transform by the view-projection inverse.
    mat4 g_ViewProjectionInverseMatrix = projection * view;
    g_ViewProjectionInverseMatrix = inverse(g_ViewProjectionInverseMatrix);
    vec4 D = H * g_ViewProjectionInverseMatrix;
    // Divide by w to get the world position.
    vec4 worldPos = D / D.w;


    // Current viewport position
    vec4 currentPos = H; // Use the world position, and transform by the previous view-
    // projection matrix.

    mat4 g_previousViewProjectionMatrix = (previousProjection*previousView);

    vec4 previousPos = vec4(worldPos * g_previousViewProjectionMatrix); // Convert to nonhomogeneous points
    // [-1,1] by dividing by w. previousPos
    // /= previousPos.w; // Use this
    // frame's position and last frame's to
    // compute the pixel
    // velocity.
    previousPos /= previousPos.w;
    vec2 velocity = vec2((currentPos - previousPos) / 2.f);

    velocity *= 0.0005f;

    //vec2 velocity = vec2(motionBlur[viewport]);
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