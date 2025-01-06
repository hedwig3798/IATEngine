#pragma once
#include "Core/Core.h"
// IEEE 754 half-precision float (16-bit) ��ȯ �Լ���
uint16 float32_to_float16(float value);
float float16_to_float32(uint16 h);

// �� ���� 16��Ʈ float�� �ϳ��� 32��Ʈ ������ ����
uint32 pack_float16_to_32(uint16 f1, uint16 f2);

// 32��Ʈ ������ �� ���� 16��Ʈ float ����
void unpack_float32_to_16(uint32 packed, uint16& OutF1, uint16& OutF2);

float PackFloat16ToFloat32(float f1, float f2);