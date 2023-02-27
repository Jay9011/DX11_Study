struct LightDesc
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;
    float3 Position;
};

cbuffer CB_Light
{
    LightDesc GlobalLight;
};

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
TextureCube SkyCubeMap;

struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

cbuffer CB_Material
{
    MaterialDesc Material;
};

MaterialDesc MakeMaterial()
{
    MaterialDesc output;
    output.Ambient = float4(0, 0, 0, 0);
    output.Diffuse = float4(0, 0, 0, 0);
    output.Specular = float4(0, 0, 0, 0);
    output.Emissive = float4(0, 0, 0, 0);

    return output;
}

float3 MaterialToColor(MaterialDesc result)
{
    return (result.Ambient + result.Diffuse + result.Specular + result.Emissive).rgb;
}

void AddMaterial(inout MaterialDesc result, MaterialDesc val)
{
    result.Ambient += val.Ambient;
    result.Diffuse += val.Diffuse;
    result.Specular += val.Specular;
    result.Emissive += val.Emissive;
}


/////////////////////////////////////////////////////////////////////

void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);

    color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

/////////////////////////////////////////////////////////////////////

void ComputeLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();

    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(direction, normalize(normal));

    output.Ambient = GlobalLight.Ambient * Material.Ambient;
    float3 E = normalize(ViewPosition() - wPosition);

    [flatten]
    if(NdotL > 0.0f)
    {
        output.Diffuse = Material.Diffuse * NdotL;

        [flatten]
        if(Material.Specular.a > 0.0f)
        {
            float3 R = normalize(reflect(-direction, normal));
            float RdotE = saturate(dot(R, E));

            float specular = pow(RdotE, Material.Specular.a);
            output.Specular = Material.Specular * specular * GlobalLight.Specular;
        }
    }
    
    [flatten]
    if(Material.Emissive.a > 0.0f)
    {
        float NdotE = dot(E, normalize(normal));
        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
        
        output.Emissive = Material.Emissive * emissive;
    }
}

///////////////////////////////////////////////////////////////////////////////

#define MAX_POINT_LIGHTS 256
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;

    float3 Position;
    float Range;

    float Intensity;
    float3 Padding;
};

cbuffer CB_PointLights
{
    uint PointLightCount;
    float3 CB_PointLights_Padding;

    PointLight PointLights[MAX_POINT_LIGHTS];
};

void ComputhPointLight(inout MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    MaterialDesc result = MakeMaterial();
    
    for (uint i = 0; i < PointLightCount; i++)
    {
        float3 light = PointLights[i].Position - wPosition;
        float dist = length(light);

        [flatten]
        if(dist > PointLights[i].Range)
            continue;

        light /= dist;  // Normalize

        // Global 대신 PointLight 에 대한 Ambient 를 계산합니다.
        result.Ambient = PointLights[i].Ambient * Material.Ambient;

        // Local Light 이기 때문에 각 정점에 대한 Light 의 방향으로 계산합니다.
        float NdotL = dot(light, normalize(normal));
        float3 E = normalize(ViewPosition() - wPosition);

        [flatten]
        if (NdotL > 0.0f)
        {
            result.Diffuse = Material.Diffuse * NdotL * PointLights[i].Diffuse;

            [flatten]
            if (Material.Specular.a > 0.0f)
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));

                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * PointLights[i].Specular; // Global 대신 PointLight 에 대한 Specular 를 계산합니다.
            }
        }
    
        [flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(E, normalize(normal));
            float emissive = smoothstep(1.0f - Material.Emissive, 1.0f, 1.0f - saturate(NdotE));
        
            result.Emissive = Material.Emissive * emissive * PointLights[i].Emissive;
        }

        float temp = 1.0f / saturate(dist / PointLights[i].Range);
        float att = temp * temp * PointLights[i].Intensity;

        output.Ambient += result.Ambient * temp;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;
    }

}