#version 450 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    float gamma = 2.2;
    vec3 diffuseColor = pow(texture(texture1, TexCoord).rgb, vec3(gamma));
    vec4 finalColor = vec4(diffuseColor.x, diffuseColor.y, diffuseColor.z, 1.0f);
    FragColor = finalColor;

    //FragColor = texture(texture1, TexCoord);
}