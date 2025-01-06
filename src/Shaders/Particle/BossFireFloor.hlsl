#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{

    // Animation Sheet UV
    float2 animationUV = g_AnimationUV_Offset + (input.UV * g_AnimationUV_Scale); 

     // �⺻ ����
    float4 baseColor = ParticleTexture0.Sample(LinearWrapSampler, animationUV);
    baseColor.rgb *= g_startColor.rgb;
    baseColor.rgb = baseColor.rgb / (baseColor.rgb + float3(1, 1, 1)); // �� ���� ���� (Reinhard �� ����)
    
    // ��� �ְ� (Normal Distortion)
    float2 normalUV;
    Ideal_TilingAndOffset_float(input.UV, float2(8, 8), float2(0, 0), normalUV);
    float3 normal = ParticleTexture1.Sample(LinearWrapSampler, normalUV).rgb; // ��� �� ���
    normal = normalize(normal * 2.0 - 1.0); // [-1, 1] ������ ��ȯ

    // ������ �κ��� �ڸ���.
    clip(baseColor.a - 0.2f);
    // ���� ���� ��ȯ
    return baseColor;
}


#endif
