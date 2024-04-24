#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;

uniform sampler2D texture1;

const int toon_color_levels = 4;
const float toon_scale_factor = 1.0f / toon_color_levels;

uniform vec3 lightPosition;

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    float ambientIntensity;

    vec3 diffuse;
    float diffuseIntensity;

    //vec3 specular;
    //float specularIntensity;
};

uniform PointLight uLight;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * light.ambientIntensity *vec3(texture(texture1, TexCoord));

    //
    float DiffuseFactor = dot(normal, -lightDir);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) {

        DiffuseFactor = ceil(DiffuseFactor * toon_color_levels) * toon_scale_factor;

        DiffuseColor = vec4(light.diffuse, 1.0f) * light.diffuseIntensity *vec4(vec3(texture(texture1, TexCoord)), 1.0f) * DiffuseFactor;
    }

    ambient *= attenuation;
    DiffuseColor *= attenuation;

    return (ambient + vec3(DiffuseColor));
}


void main() {
    vec3 norm = normalize(vec3(0.0f, -1.0f, 0.0f));
    vec3 result = vec3(0,0,0);

    //PointLight pointLight;
    //pointLight.ambient = vec3(1.0f, 1.0f, 1.0f);
    //pointLight.ambientIntensity = 1.0f;
    //pointLight.constant = 1.0f;
    //pointLight.diffuse = vec3(0.7f, 0.7f, 0.7f);
    //pointLight.diffuseIntensity = 5.0f;
    //pointLight.linear = 0.1f;
    //pointLight.quadratic = 0.1f;
    //pointLight.position = lightPosition;
    //pointLight.position = light.position;

    result = CalcPointLight(uLight, norm, FragPos);
    FragColor = vec4(result, 1.0);
}