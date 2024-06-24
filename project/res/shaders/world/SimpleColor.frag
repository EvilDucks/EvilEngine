#version 450 core
out vec4 FragColor;

uniform vec4 color;
in vec3 fg_pos;
in vec3 fg_normal;
in vec2 fg_uv;

const int toon_color_levels = 4;
const float toon_scale_factor = 1.0f / toon_color_levels;
uniform vec3 lightPosition = vec3(1.0, 1.0, 1.0);
uniform vec3 camPos = vec3(0.0, 0.0, 0.0);

struct BaseLight {
    vec3 ambient;
    float ambientIntensity;
    vec3 diffuse;
    float diffuseIntensity;
    vec3 specular;
    float specularIntensity;
};

struct Attenuation
{
    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    bool flag;
    vec3 position;

    Attenuation attenuation;

    BaseLight base;
};

uniform PointLight uLight;

struct SpotLight {
    bool flag;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    Attenuation attenuation;

    BaseLight base;
};

struct DirLight {
    bool flag;
    vec3 direction;

    BaseLight base;
};

vec4 CelShading(BaseLight light, vec3 lighDirection, vec3 normal)
{
    vec4 ambient = vec4(light.ambient, 1.0f) * light.ambientIntensity;

    float DiffuseFactor = dot(normal, -lighDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    // Change to uniform later
    bool gCellShadingEnabled = true;

    if (DiffuseFactor > 0) {
        if (gCellShadingEnabled) {
            DiffuseFactor = ceil(DiffuseFactor * toon_color_levels) * toon_scale_factor;
        }

        DiffuseColor = vec4(light.diffuse, 1.0f) * light.diffuseIntensity * DiffuseFactor;
    }

    return DiffuseColor + ambient;
}

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.attenuation.constant + light.attenuation.linear * distance + light.attenuation.quadratic * (distance * distance));

    vec4 result = CelShading(light.base, lightDir, normal);

    result *= attenuation;

    return result;
}

vec4 CalcDirectionalLight(DirLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(-light.direction);
    //-light direction

    vec4 result = CelShading(light.base, lightDir, normal);

    return result;
}

void main() {
    //PointLight lightUniform;
    //lightUniform.position = vec3(1.0, 1.0, 1.0);
    //lightUniform.attenuation.constant = 1.0; w
    //lightUniform.attenuation.linear = 0.1;
    //lightUniform.attenuation.quadratic = 0.1;
    //lightUniform.base.ambient = vec3(1.0, 1.0, 1.0);
    //lightUniform.base.ambientIntensity = 1.0;
    //lightUniform.base.diffuse = vec3(0.7, 0.7, 0.7);
    //lightUniform.base.diffuseIntensity = 0.1;

// fog
    float distance = length(camPos - fg_pos)*0.01;
    float density = 1.2;
    float fogFactor = exp(-density * density * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 fog = vec4(0.5, 0.5, 0.5, 1.0);
    vec4 result = color;
    result = CalcPointLight(uLight, fg_normal, fg_pos);
    FragColor = mix(fog, color, fogFactor);
} 