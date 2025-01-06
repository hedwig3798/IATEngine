#pragma once
#include <memory>
#include <string>

namespace Ideal
{
	class IMaterial;
}

namespace Ideal
{
	class IMesh
	{
	public:
		IMesh() {}
		virtual ~IMesh() {}

	public:
		virtual std::string GetName() abstract;
		// �Ʒ��� Set Material�� Particle�� ȣȯ�� �ȵ�.
		virtual void SetMaterialObject(std::shared_ptr<Ideal::IMaterial> Material) abstract;

		// ó������ �⺻���� material�� ����ϱ� �ϴµ� ���� ���� ���׸����� ����ϴ��� �̾��ش�.
		virtual std::string GetFBXMaterialName() abstract;
		// ���߿� �̰� ���ٵ�
		virtual std::weak_ptr<Ideal::IMaterial> GetMaterialObject() abstract;
	};
}