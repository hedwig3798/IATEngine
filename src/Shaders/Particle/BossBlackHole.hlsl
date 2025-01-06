#ifndef TEST_CUSTOM_PARTICLE
#define TEST_CUSTOM_PARTICLE

#include "ParticleCommon.hlsli" // necessary

float4 PSMain(VSOutput input) : SV_TARGET
{
    // UV ��ǥ
    float2 UV = input.UV;
 // �ð� ��� �ִϸ��̼�
    float timeFactor = g_Time * 0.1f;// * g_CustomData2.w;
    UV += float2(timeFactor, 0); // UV ��ũ�Ѹ�

    float2 mainTexUV;
    Ideal_TilingAndOffset_float(UV, float2(1,2), float2(0,0), mainTexUV);
    // �⺻ �ؽ�ó ���ø�
    float4 mainTexColor = ParticleTexture0.Sample(LinearWrapSampler, mainTexUV);
    mainTexColor.rgb *= g_startColor.rgb * 7.5; //factor
    //mainTexColor.rgb = mainTexColor.rgb / (mainTexColor.rgb + float3(1, 1, 1)); // �� ���� ���� (Reinhard �� ����)

    // �븻 �ؽ�ó ���ø�
    float4 normalTexColor = ParticleTexture1.Sample(LinearWrapSampler, UV);
    
    // �븻 ���ͷ� ��ȯ
    float3 normal = normalize(normalTexColor.rgb * 2.0 - 1.0); // [0,1] ������ [-1,1]�� ��ȯ

    // �븻 �ְ� ó��
    {
        normal *= 0.5; // �븻 �ְ� ���� ����
    }

    // ������ �ؽ�ó �߰�
    float4 noiseTexColor = ParticleTexture2.Sample(LinearWrapSampler, UV); // ������ �ؽ�ó ���ø�

   
    // ����ũ ���̵� ó��
    float maskFade = 1.0;
    //if (g_CustomData1.w > 0) // ����ũ ���̵� Ȱ��ȭ
    {
        float4 maskTexColor = ParticleTexture1.Sample(LinearWrapSampler, UV); // ����ũ �ؽ�ó ���ø�
        maskFade = maskTexColor.a; // ���İ����� ���̵� �� ����
    }

    // ���� ���� ���
    float4 finalColor = mainTexColor * g_startColor * maskFade;

    // ���� ó��
    finalColor.a *= mainTexColor.a;

    // ����Ʈ ����
    finalColor = clamp(finalColor, 0.0, 1.0); // ���� �� ����

    return finalColor;
}


#endif
