#version 330 core
out vec4 FragColor;

in vec3 fg_pos;

uniform vec3 camPos = vec3(0.0, 0.0, 0.0);

const int toon_color_levels = 10;
const float toon_scale_factor = 1.0 / toon_color_levels;

const int lightCount = 45;
vec3 lightPositions[lightCount] = vec3[](
//Box 1 - 24 lights
//Layer1
vec3(0, 8, -13), vec3(-12, 8, -13), vec3(0, 8, -37), vec3(-12, 8, -37),
vec3(-12, 8, -25), vec3(12, 8, -13), vec3(12, 8, -25), vec3(12, 8, -37),
// L2
//vec3(0, 16, -13), vec3(-12, 16, -13), vec3(0, 16, -37), vec3(-12, 16, -37),
//vec3(-12, 16, -25), vec3(12, 16, -13), vec3(12, 16, -25), vec3(12, 16, -37),
// L3
//vec3(0, 24, -13), vec3(-12, 24, -13), vec3(0, 24, -37), vec3(-12, 24, -37),
//vec3(-12, 24, -25), vec3(12, 24, -13), vec3(12, 24, -25), vec3(12, 24, -37),
// Box 2 - 48 lights
//Layer1
vec3(0, 32, -13), vec3(-12, 32, -13), vec3(0, 32, -37), vec3(-12, 32, -37),
vec3(-12, 32, -25), vec3(12, 32, -13), vec3(12, 32, -25), vec3(12, 32, -37),
// L2
//vec3(0, 40, -13), vec3(-12, 40, -13), vec3(0, 40, -37), vec3(-12, 40, -37),
//vec3(-12, 40, -25), vec3(12, 40, -13), vec3(12, 40, -25), vec3(12, 40, -37),
// L3
//vec3(0, 48, -13), vec3(-12, 48, -13), vec3(0, 48, -37), vec3(-12, 48, -37),
//vec3(-12, 48, -25), vec3(12, 48, -13), vec3(12, 48, -25), vec3(12, 48, -37),
// Box 3 - 72 lights
vec3(0, 56, -13), vec3(-12, 56, -13), vec3(0, 56, -37), vec3(-12, 56, -37),
vec3(-12, 56, -25), vec3(12, 56, -13), vec3(12, 56, -25), vec3(12, 56, -37),
// L2
//vec3(0, 64, -13), vec3(-12, 64, -13), vec3(0, 64, -37), vec3(-12, 64, -37),
//vec3(-12, 64, -25), vec3(12, 64, -13), vec3(12, 64, -25), vec3(12, 64, -37),
// L3
//vec3(0, 72, -13), vec3(-12, 72, -13), vec3(0, 72, -37), vec3(-12, 72, -37),
//vec3(-12, 72, -25), vec3(12, 72, -13), vec3(12, 72, -25), vec3(12, 72, -37),
// Box 4 - 96 lights
vec3(0, 80, -13), vec3(-12, 80, -13), vec3(0, 80, -37), vec3(-12, 80, -37),
vec3(-12, 80, -25), vec3(12, 80, -13), vec3(12, 80, -25), vec3(12, 80, -37),
// L2
//vec3(0, 88, -13), vec3(-12, 88, -13), vec3(0, 88, -37), vec3(-12, 88, -37),
//vec3(-12, 88, -25), vec3(12, 88, -13), vec3(12, 88, -25), vec3(12, 88, -37),
// L3
//vec3(0, 96, -13), vec3(-12, 96, -13), vec3(0, 96, -37), vec3(-12, 96, -37),
//vec3(-12, 96, -25), vec3(12, 96, -13), vec3(12, 96, -25), vec3(12, 96, -37),
// Box 5 - 120 lights
vec3(0, 104, -13), vec3(-12, 104, -13), vec3(0, 104, -37), vec3(-12, 104, -37),
vec3(-12, 104, -25), vec3(12, 104, -13), vec3(12, 104, -25), vec3(12, 104, -37),
// L2
//vec3(0, 112, -13), vec3(-12, 112, -13), vec3(0, 112, -37), vec3(-12, 112, -37),
//vec3(-12, 112, -25), vec3(12, 112, -13), vec3(12, 112, -25), vec3(12, 112, -37),
// L3
//vec3(0, 120, -13), vec3(-12, 120, -13), vec3(0, 120, -37), vec3(-12, 120, -37),
//vec3(-12, 120, -25), vec3(12, 120, -13), vec3(12, 120, -25), vec3(12, 120, -37),
vec3(10, 3, 10), vec3(10, 3, -10), vec3(-10, 3, 10), vec3(-10, 3, -10), vec3(0, 123, 0)
);

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

vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 lightPos)
{
    vec3 lightDir = normalize(lightPos - fragPos);

    // attenuation
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (
        light.attenuation.constant +
        light.attenuation.linear * distance +
        light.attenuation.quadratic * (distance * distance)
    );

    vec4 result = CelShading(light.base, lightDir, normal);

    result *= attenuation;

    return result;
}

void main() {
	vec3 dx = dFdx(fg_pos);
	vec3 dy = dFdy(fg_pos);
	vec3 normal = normalize(cross(dx, dy));

// fog
    float distance = length(camPos - fg_pos)*0.01;
    float density = 1.2;
    float fogFactor = exp(-density * density * distance);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec4 fogColor = vec4(0.5, 0.5, 0.5, 1.0);

	//FragColor = mix(fog, vec4(1.0f, 0.647f, 0.0f, 1.0f), fogFactor);

	 vec4 lightColor = vec4(0, 0, 0, 1);

        for(int i = 0; i < lightCount; i++)
        {
         lightColor += CalcPointLight(uLight, normal, fg_pos, lightPositions[i]);
        }
        FragColor = mix(fogColor, vec4(1.0f, 0.647f, 0.0f, 1.0f) * lightColor, fogFactor);
        //FragColor = mix(fogColor, vec4(238.0/255.0, 230.0/255.0f, 217.0/255.0, 1.0) * lightColor, fogFactor);
} 