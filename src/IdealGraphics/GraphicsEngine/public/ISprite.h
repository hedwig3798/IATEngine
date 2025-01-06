#pragma once
#include "../Utils/SimpleMath.h"
#include <memory>

namespace Ideal
{
	class ITexture;
}

namespace Ideal
{
	// Texture���� �ڸ� ����
	// Texture�� ��ǥ�� 0,0 �� ���� �� , ������ �Ʒ��� ������.
	struct SpriteRectArea
	{
		SpriteRectArea()
			: x(0), y(0), width(1), height(1)
		{}
		SpriteRectArea(
			unsigned int posX, unsigned int posY, unsigned int RectWidth, unsigned int RectHeight
		)
			: x(posX), y(posY), width(RectWidth), height(RectHeight) {}
		SpriteRectArea operator=(const SpriteRectArea& other)
		{
			x = other.x;
			width = other.width;
			y = other.y;
			height = other.height;
		}
		unsigned int x;
		unsigned int width;
		unsigned int y;
		unsigned int height;
	};

	class ISprite
	{
	public:
		ISprite() {}
		virtual ~ISprite() {}

	public:
		// Draw On Off
		virtual void SetActive(bool) abstract;
		virtual bool GetActive() abstract;

	public:
		virtual DirectX::SimpleMath::Vector2 const& GetPosition() abstract;
		virtual float GetZ() abstract;
		virtual float GetAlpha() abstract;
		virtual DirectX::SimpleMath::Color const& GetColor() abstract;
		// �ؽ��Ŀ��� ������ �簢��(����)�� ���� ���
		virtual void SetSampleRect(const SpriteRectArea&) abstract;
		virtual void SetPosition(const DirectX::SimpleMath::Vector2&) abstract;
		virtual void SetScale(const DirectX::SimpleMath::Vector2&) abstract;
		virtual void SetZ(float) abstract;
		virtual void SetAlpha(float) abstract;
		virtual void SetColor(const DirectX::SimpleMath::Color&) abstract;
		virtual void SetTexture(std::weak_ptr<Ideal::ITexture>) abstract;
	};
}