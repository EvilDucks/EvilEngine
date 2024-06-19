#version 450 core
out vec4 FragColor;

in vec3 textureCoords;

uniform samplerCube skybox;

void main()
{    
    //FragColor = texture(skybox, textureCoords);

    float gamma = 2.2;

    vec3 color = texture(skybox, textureCoords).rgb;
	vec3 resultGamma = pow(color, vec3(gamma));
	
	FragColor = vec4(color, 1.0f);
}