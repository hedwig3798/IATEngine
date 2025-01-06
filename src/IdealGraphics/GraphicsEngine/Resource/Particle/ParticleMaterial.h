#pragma once
#include "IParticleMaterial.h"
#include "Core/Core.h"

struct ID3D12Device;

namespace Ideal
{
	class D3D12Shader;
	class D3D12Texture;
}

/// <summary>
/// ��� 1 
/// UV.z �� Curve�����͸� �ִ� ���
/// BitPacking �� �̿��Ͽ� 16bit float�� �� ���� �ְ� �ð��� ���� ������ ����Ѵ�.
/// �̷��� �Ǹ� Root Signature�� PSO�� �ٸ� ������� ����.. �ʤ���...
/// PSO�� ��·�� ���̴� ������ �޶����� ������ Vertex�� ������ ���ϵǰ�
/// Constant Buffer�� �� �߰����� �ʾƵ� �ȴ�.
/// 
/// �׷��ٸ� �ؾ� �� ��
/// ������ ���� UV�� float4�� �ٲٰ� UV.zw ���� �� �� �ְ� �ؾ� �Ѵ�.
/// Vertex Data
/// {
///		Position, Normal, Tangent, UV0, UV1, Color
/// }
/// 
/// �ؽ��Ŀ� �̹����� �´� UV��ǥ�� ���� �־�� �Ѵ�.
/// 
/// ���2
/// constant buffer�� �� �����ڴ´�.
/// </summary>
namespace Ideal
{
	class ParticleMaterial : public IParticleMaterial
	{
	public:
		ParticleMaterial() {};
		virtual ~ParticleMaterial() {};

	public:
		virtual void SetBlendingMode(Ideal::ParticleMaterialMenu::EBlendingMode BlendingMode) override;
		virtual void SetShader(std::shared_ptr<Ideal::IShader> Shader) override;
		virtual void SetShaderByPath(const std::wstring& FilePath) override;
		virtual void SetTexture0(std::shared_ptr<Ideal::ITexture> Texture) override;
		virtual void SetTexture1(std::shared_ptr<Ideal::ITexture> Texture) override;
		virtual void SetTexture2(std::shared_ptr<Ideal::ITexture> Texture) override;
		virtual void SetBackFaceCulling(bool Culling) override;
		virtual void SetWriteDepthBuffer(bool Write) override;
		virtual void SetTransparency(bool Transpaerancy) override;
	public:
		Ideal::ParticleMaterialMenu::EBlendingMode GetBlendingMode();
		std::shared_ptr<Ideal::D3D12Shader> GetShader();
		std::weak_ptr<Ideal::D3D12Texture> GetTexture0();
		std::weak_ptr<Ideal::D3D12Texture> GetTexture1();
		std::weak_ptr<Ideal::D3D12Texture> GetTexture2();
		bool GetBackFaceCulling();
		bool GetWriteDepthBuffer();
		bool GetTransparency();

	private:
		Ideal::ParticleMaterialMenu::EBlendingMode m_blendingMode;
		std::shared_ptr<Ideal::D3D12Shader> m_shader;	// ���̴� �ý���
		std::weak_ptr<Ideal::D3D12Texture> m_texture0;	
		std::weak_ptr<Ideal::D3D12Texture> m_texture1;	
		std::weak_ptr<Ideal::D3D12Texture> m_texture2;	
		bool m_isBackFaceCulling = true;
		bool m_isWriteDepthBuffer = false;
		bool m_isTransperancy = false;
	};
}