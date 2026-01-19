struct Input {
	float3 pos : POSITION0;
    float3 col : COLOR0;
};

struct Output {
	float4 pos : SV_POSITION;
    float3 col : COLOR0;
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
    output.col = input.col;

	return output;
}