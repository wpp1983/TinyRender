struct VertexInput {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInput VSMain(VertexInput input) {
    PixelInput output;
    output.position = float4(input.position, 1.0f);
    output.color = input.color;
    return output;
}

float4 PSMain(PixelInput input) : SV_TARGET {
    return input.color;
} 