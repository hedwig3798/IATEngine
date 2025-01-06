#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 output;

    // ī�޶� ��ġ�� View ��Ŀ��� ���
    float4 cameraPosInWorld = float4(eyePos, 1); // View ��Ŀ��� ī�޶� ��ġ ����
    float3 viewDir = normalize(cameraPosInWorld.xyz - input.Pos); // ī�޶� ����

    // ������ ī�޶� ���� ���� ���� ���
    float rim = saturate(dot(input.Normal, viewDir)); // ������ ���� ���� ���
    rim = pow(1 - rim, 2);

    // ������Ʈ ȿ���� ���� ���� ����
    output = rim * g_startColor;

    output.a = 1; // ���� �� ���� (������)
    
    return output;
}


#endif
