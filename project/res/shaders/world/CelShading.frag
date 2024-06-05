#version 450 core
out vec4 FragColor;

in vec3 fg_pos;
in vec3 fg_normal;
in vec2 fg_uv;

uniform sampler2D texture1;

const int toon_color_levels = 4;
const float toon_scale_factor = 1.0f / toon_color_levels;

uniform vec3 lightPosition;

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
    vec4 ambient = vec4(light.ambient, 1.0f) * light.ambientIntensity * vec4(texture(texture1, fg_uv));

    float DiffuseFactor = dot(normal, -lighDirection);

    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    // Change to uniform later
    bool gCellShadingEnabled = true;

    if (DiffuseFactor > 0) {
        if (gCellShadingEnabled) {
            DiffuseFactor = ceil(DiffuseFactor * toon_color_levels) * toon_scale_factor;
        }

        DiffuseColor = vec4(light.diffuse, 1.0f) * light.diffuseIntensity * vec4(vec3(texture(texture1, fg_uv)), 1.0f) * DiffuseFactor;

        //        vec3 PixelToCamera = normalize(gCameraLocalPos - LocalPos0);
        //        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        //        float SpecularFactor = dot(PixelToCamera, LightReflect);
        //
        //        if (!gCellShadingEnabled && (SpecularFactor > 0)) {
        //            float SpecularExponent = 128.0;
        //
        //            if (gEnableSpecularExponent) {
        //                SpecularExponent = texture2D(gSamplerSpecularExponent, fg_uv).r * 255.0;
        //            }
        //
        //            SpecularFactor = pow(SpecularFactor, SpecularExponent);
        //            SpecularColor = vec4(Light.Color, 1.0f) *
        //            Light.DiffuseIntensity * // using the diffuse intensity for diffuse/specular
        //            vec4(gMaterial.SpecularColor, 1.0f) *
        //            SpecularFactor;
        //        }

        //        if (gRimLightEnabled) {
        //            float RimFactor = CalcRimLightFactor(PixelToCamera, Normal);
        //            RimColor = DiffuseColor * RimFactor;
        //        }
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
    vec4 result = vec4(0,0,0,1);

    result = CalcPointLight(uLight, fg_normal, fg_pos);
    FragColor = vec4(vec3(result), 1.0f);
}