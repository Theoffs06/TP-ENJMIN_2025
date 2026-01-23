Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

cbuffer LightData : register (b0) {
    float3 direction;
    float3 ambientColor;
    float3 diffuseColor;
}

float4 main(Input input) : SV_TARGET {
    float4 res = tex.Sample(samplerState, input.uv);

    float3 lightIntensity = ambientColor; // ambient
    lightIntensity += saturate(dot(input.normal, direction)) * diffuseColor; // diffuse
    res.rgb *= saturate(lightIntensity);

    return res;
}