#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    // Pixel Shader
    float2 uv = input.UV;
    float2 tiling0 = float2(2, 2);
    float2 offset0 = float2(0, 0);
    
    //float timeOffset = g_Time; // �ð��� ���� ������
    uv += g_Time;
    // uv += float2(sin(timeOffset) * 0.1, cos(timeOffset) * 0.1); // ������

    
    Ideal_TilingAndOffset_float(uv, tiling0, offset0, uv);

    float4 colorTexture = ParticleTexture0.Sample(LinearWrapSampler, uv);
    float4 noiseTexture = ParticleTexture1.Sample(LinearWrapSampler, uv);
    
    // ������ �ؽ�ó�� R ä���� ����Ͽ� �÷� �ؽ�ó�� ���� ���� ����
    float alpha = noiseTexture.r; // ��� �ؽ�ó�� R ä���� ���ķ� ���
    float4 finalColor = colorTexture * alpha; // ����� �̿��� ȥ��
    
    // cbuffer���� ������ ���� ���
    float4 startColor = g_startColor; // ParticleSystemData���� ���� ��������

    // ���� ���� �ʱ� ���� ����
    finalColor.rgb *= startColor.rgb; // ���� ����
    
    // �߰����� ȿ�� (��: ���� ��ȭ)
    //float hueOffset = g_Time * 0.1; // �ð��� ���� ���� ��ȭ
    //Ideal_Hue_Degrees_float(finalColor.rgb, hueOffset, finalColor.rgb);

    return finalColor;
}


#endif
