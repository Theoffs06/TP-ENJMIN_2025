struct Input {
	float3 pos : POSITION0;
    float2 uv : TEXCOORD0;
};

struct Output {
	float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer ModelData : register(b0) {
    float4x4 model;
};

cbuffer CameraData : register(b1) {
    float4x4 view;
    float4x4 projection;
};

Output main(Input input) {
	Output output = (Output)0;

    output.pos = mul(float4(input.pos, 1), model);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = input.uv;

	return output;
}