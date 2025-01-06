#include "BitUtils.h"


uint16 float32_to_float16(float value)
{
	uint32 f = 0;

	// float -> uint32�� ��ȯ
	std::memcpy(&f, &value, sizeof(float));

	uint16 sign = (f >> 16) & 0x8000; // ��ȣ ��Ʈ ����
	uint16 exponent = ((f >> 23) & 0xFF) - (127 - 15); // ���� ��ȯ
	uint16 mantissa = (f >> 13) & 0x3FF; // ���� ��ȯ

	if (exponent <= 0)
	{
		// ����÷ο� ó��
		exponent = 0;
	}
	else if (exponent >= 31)
	{
		// �����÷ο� ó�� (INF, NAN)
		exponent = 31;
		mantissa = 0;
	}

	return sign | (exponent << 10) | mantissa;
}

float float16_to_float32(uint16 h)
{
	uint32 sign = (h & 0x8000) << 16;	// ��ȣ ��Ʈ ����
	uint32 exponent = (h & 0x700) >> 10; // ���� ����
	uint32 mantissa = (h & 0x03FF);	// ���� ����
	
	if (exponent == 0)
	{
		// ������ �Ǵ� 0 ó��
		exponent = 0;
	}
	else if (exponent == 31)
	{
		// Inf �Ǵ� NaN ó��
		exponent = 255;
	}
	else
	{
		exponent = exponent + (127 - 15);	// ���� ������
	}

	uint32 f = sign | (exponent << 23) | (mantissa << 13);
	float result;
	std::memcpy(&result, &f, sizeof(float));	// uint32 -> float ��ȯ
	return result;
}

uint32 pack_float16_to_32(uint16 f1, uint16 f2)
{
	return (static_cast<uint32>(f1) << 16) | f2;
}

void unpack_float32_to_16(uint32 packed, uint16& OutF1, uint16& OutF2)
{
	OutF1 = static_cast<uint16>(packed >> 16);
	OutF2 = static_cast<uint16>(packed & 0xFFFF);
}

float PackFloat16ToFloat32(float f1, float f2)
{
	uint16 f1_16 = float32_to_float16(f1);
	uint16 f2_16 = float32_to_float16(f2);

	uint32 packed = pack_float16_to_32(f1_16, f2_16);

	float packedFloat;
	std::memcpy(&packedFloat, &packed, sizeof(packedFloat));	// uint32�� float�� ��Ʈ ������ ��ȯ

	return packedFloat;
}
