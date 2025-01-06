#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.UV;
    float2 maskUV = input.UV;

    // �ð��� ���� �ִϸ��̼��� ���� UV ��ȯ
    float scrollSpeed = 2.5; // ��ũ�� �ӵ� ����
    //float scrollSpeed = 5; // ��ũ�� �ӵ� ����
    Ideal_TilingAndOffset_float(maskUV, float2(1,-1), float2(0,0), maskUV);
    uv.y += g_Time * scrollSpeed; // �ð��� ���� Y�� �������� ��ũ��
    
    // ����ũ �ؽ�ó�� ȭ�� �ؽ�ó ���ø�
    float4 maskColor = ParticleTexture0.Sample(LinearWrapSampler, maskUV);
    float4 fireColor = ParticleTexture1.Sample(LinearWrapSampler, uv);
    
    // ȥ�� ȿ��: ����ũ ���� ���� ȭ�� ������ ����
    float4 outputColor = g_startColor * fireColor * maskColor.a; // ���İ��� ����Ͽ� ȥ��
    outputColor.rgb *= maskColor.rgb;
    // ��� ���� ��ȯ
    //float4 finalColor = float4(outputColor.rgb, 1);
    //finalColor.rgb *= g_startColor;
    //return finalColor;
    return outputColor;
}


#endif
