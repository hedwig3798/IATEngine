#pragma once
#include "IRendererResource.h"
#include "../Utils/SimpleMath.h"
#include <memory>

namespace Ideal
{
	class IMesh;
	class IBone;
}

namespace Ideal
{
	enum EMeshType
	{
		Static,
		Skinned
	};

	class IMeshObject : public IRendererResource
	{
	public:
		IMeshObject() {}
		virtual ~IMeshObject() {}

	public:
		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Matrix) abstract;
		virtual void SetDrawObject(bool IsDraw) abstract;

		virtual DirectX::SimpleMath::Matrix GetLocalTransformMatrix() abstract;

		virtual Ideal::EMeshType GetMeshType() const abstract;

		// �Ž��� �����ϋ��� üũ // ����ȭ �뵵�� ���
		virtual void SetStaticWhenRunTime(bool Static) abstract;

		// ����� �Ž��϶��� �۵�, ��Ű�� �Ž��� �۵� ����
		virtual void SetDebugMeshColor(DirectX::SimpleMath::Color& Color) abstract;

		virtual void AlphaClippingCheck() abstract;

	public:
		virtual unsigned int GetMeshesSize() abstract;
		virtual std::weak_ptr<Ideal::IMesh> GetMeshByIndex(unsigned int) abstract;
		
		virtual unsigned int GetBonesSize() abstract;
		virtual std::weak_ptr<Ideal::IBone> GetBoneByIndex(unsigned int) abstract;

	public:
		virtual void AddLayer(unsigned int LayerNum) abstract;
		virtual void DeleteLayer(unsigned int LayerNum) abstract;
		virtual void ChangeLayer(unsigned int LayerNum) abstract;
	};
}