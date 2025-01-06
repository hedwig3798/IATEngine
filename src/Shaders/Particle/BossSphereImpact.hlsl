#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{  // �⺻ ����
     // �⺻ ����
    float4 baseColor = ParticleTexture0.Sample(LinearWrapSampler, input.UV);
    
    baseColor.rgb *= g_startColor.rgb;
    baseColor.rgb = baseColor.rgb / (baseColor.rgb + float3(1, 1, 1)); // �� ���� ���� (Reinhard �� ����)

    baseColor.a *= g_startColor.a;

    // Color Factor
    baseColor.rgb *= g_CustomData1.z;

    // �ð��� ���� �ִϸ��̼� ���� (Texture Animate)
    float2 animatedUV = input.UV;
    float2 animatedUVTO;
    Ideal_TilingAndOffset_float(input.UV, float2(g_CustomData1.y, 2), float2(0, 0), animatedUVTO);    // offet customaData���� �ҷ���
    animatedUVTO += g_Time * g_CustomData1.x; // �ִϸ��̼� �ӵ� ����
    baseColor *= ParticleTexture0.Sample(LinearWrapSampler, animatedUVTO); // �ؽ�ó �ִϸ��̼� ����

    // ��� �ְ� (Normal Distortion)
    float2 normalUV;
    Ideal_TilingAndOffset_float(input.UV, float2(2, 2), float2(0, 0), normalUV);
    float3 normal = ParticleTexture1.Sample(LinearWrapSampler, normalUV).rgb; // ��� �� ���
    normal = normalize(normal * 2.0 - 1.0); // [-1, 1] ������ ��ȯ

    // ����ũ ���̵� (Mask Fade)
    float2 maskUV;
    Ideal_TilingAndOffset_float(input.UV, float2(1, -1), float2(0, 0), maskUV);
    float4 maskAlpha = ParticleTexture2.Sample(LinearWrapSampler, maskUV); // ����ũ �ؽ�ó ���

    baseColor *= maskAlpha;
    
    // ���� ���� ��ȯ
    return baseColor;
}


#endif
