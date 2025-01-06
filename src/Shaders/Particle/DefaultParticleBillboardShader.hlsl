#ifndef DEFAULT_PARTICLE_BILLBOARD_VS_HLSL
#define DEFAULT_PARTICLE_BILLBOARD_VS_HLSL

#include "ParticleCommon.hlsli" // necessary

VSParticleDrawOut VSMain(VSParticleInput input)
{
    VSParticleDrawOut output;
    //output.Pos = float3(0,0,0);
    //output.Pos = float3(input.ID * 10,0,0);
    output.Pos = g_bufPos[input.ID].Position.xyz;
    float rotationAngle = g_bufPos[input.ID].RotationAngle;
    output.Color = float4(rotationAngle,1,1,1);
    return output;
}

[maxvertexcount(4)]
void GSMain(point VSParticleDrawOut input[1], inout TriangleStream<VSOutput> SpriteStream)
{
    VSOutput output;
    // �����带 ����ڴ�
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = eyePos - input[0].Pos;
    //look.y = 0.0f;  // y �� �����̹Ƿ� xz ��鿡 ����
    look = normalize(look);
    float3 right = cross(up, look);
    up = cross(look, right);
    up = normalize(up);
    right = normalize(right);
    float halfWidth = 0.5f * g_ParticleSize.x; // * input[0].Size.x // TODO : ���߿� ������ �߰� �� ��
    float halfHeight = 0.5f * g_ParticleSize.y; // * input[0].Size.y // TODO : ���߿� ������ �߰� �� ��

    //////  Angle   //////
    //float rotationAngle = 1.57f; // TEMP
    float rotationAngle = input[0].Color.r;
    float cosAngle = cos(rotationAngle);
    float sinAngle = sin(rotationAngle);
    
    float2x2 rotationMatrix = float2x2(cosAngle, -sinAngle, sinAngle, cosAngle);
    
    // �������� �� �������� ���(ȸ�� ����)
    float2 localOffset[4] = {
        float2(halfWidth, -halfHeight),  // ������ �Ʒ�
        float2(halfWidth, halfHeight),   // ������ ��
        float2(-halfWidth, -halfHeight), // ���� �Ʒ�
        float2(-halfWidth, halfHeight)   // ���� ��
    };

    for(int i = 0; i < 4; i++)
    {
        // ȸ�� ��� ����
        float2 rotatedOffset = mul(localOffset[i], rotationMatrix);
        float3 billboardPos = input[0].Pos + (rotatedOffset.x * right) + (rotatedOffset.y * up);
    
        output.PosH = mul(float4(billboardPos, 1.0), ViewProj);
        output.Pos = input[0].Pos.xyz;
        //output.Color = input[0].Color;
        // TEMP : �븻�� ����
        output.Normal = float3(1.f,0.f,0.f);
        output.UV = g_texcoords[i];
        SpriteStream.Append(output);
    }
    SpriteStream.RestartStrip();    // �ﰢ�� ����� �ʱ�ȭ
}

// TEST
float4 PSMain(VSOutput input) : SV_Target
{
    float4 ret;
    float2 animationUV = g_AnimationUV_Offset + (input.UV * g_AnimationUV_Scale); 
    //ret = ParticleTexture0.Sample(LinearWrapSampler, input.UV);
    ret = ParticleTexture0.Sample(LinearWrapSampler, animationUV);
    clip(ret.a - 0.1f);
    return ret;
}

#endif
