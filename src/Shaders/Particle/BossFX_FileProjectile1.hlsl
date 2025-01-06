#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    float2 uv = input.UV;
    float2 maskUV = input.UV;

    // �ð��� ���� �ִϸ��̼��� ���� UV ��ȯ
    float scrollSpeed = 2.5; // ��ũ�� �ӵ� ����
    uv.y += g_Time * scrollSpeed; // ȭ�� �ؽ�ó�� Y�� ��ũ��

    // ����ũ UV�� ������ ���̴� ȿ��
    maskUV.y -= g_Time * 0.5; // ����ũ�� �������� �̵��Ͽ� ������ ���̴� ������ �ֱ� ���� ����

    // �ؽ�ó ���ø�
    float4 maskColor = ParticleTexture0.Sample(LinearWrapSampler, maskUV); // ����ũ �ؽ�ó
    float4 fireColor = ParticleTexture1.Sample(LinearWrapSampler, uv); // ȭ�� �ؽ�ó
    float4 normalMapColor = ParticleTexture2.Sample(LinearWrapSampler, uv); // ��� ��

    // ���İ� ���� ó��
    float invertedAlpha = 1.0 - maskColor.a; 

    // ȥ�� ȿ��: ȭ�� ���� ������ ���İ��� ������
    float4 outputColor = fireColor * invertedAlpha; 
    outputColor.rgb *= maskColor.rgb; // ����ũ �������� ȭ�� ���� ����

    // ��� ���� ����Ͽ� ���� ���� �߰�
    outputColor.rgb += normalMapColor.rgb * 0.2; // ��� ���� ������ �߰�

    // ��� ���� ��ȯ
    return outputColor;
}


#endif
