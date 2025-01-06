#pragma once
#include "IParticleSystem.h"
#include "Core/Core.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/ConstantBufferInfo.h"

struct ID3D12Device;
struct ID3D12RootSignature;
struct ID3D12PipelineState;
struct ID3D12GraphicsCommandList;

namespace Ideal
{
	class ParticleMaterial;
	class IMesh;
	template <typename> class IdealMesh;

	class D3D12Shader;
	class D3D12PipelineStateObject;
	class D3D12DescriptorHeap;
	class D3D12DynamicConstantBufferAllocator;
	class D3D12VertexBuffer;
	class D3D12StructuredBuffer;
	class ResourceManager;
	class DeferredDeleteManager;
	class IdealCamera;
}

namespace Ideal
{
	class ParticleSystem : public IParticleSystem
	{
		// TODO
	public:
		ParticleSystem();
		virtual ~ParticleSystem();

	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) override;
		virtual std::weak_ptr<Ideal::IParticleMaterial> GetMaterial() override;

		virtual void SetTransformMatrix(const Matrix& Transform) override;
		virtual const Matrix& GetTransformMatrix() const override;

		virtual void SetActive(bool IsActive) override;
		virtual bool GetActive() override;

		virtual void SetDeltaTime(float DT) override;
		
		virtual void Play() override;
		virtual void Resume() override;	// �Ͻ� ���� ���� ��� ���� �ð����� ����
		virtual void Pause() override;
		virtual void SetStopWhenFinished(bool StopWhenFinished) override;
		virtual void SetPlayOnWake(bool PlayOnWake) override;
		virtual float GetCurrentDurationTime() override;
		virtual void SetMaxParticles(unsigned int MaxParticles) override;

	public:
		void Init(ComPtr<ID3D12Device> Device, ComPtr<ID3D12RootSignature> RootSignature, std::shared_ptr<Ideal::ParticleMaterial> ParticleMaterial);
		void Free();
		void SetResourceManager(std::shared_ptr<Ideal::ResourceManager> ResourceManager);
		void SetDeferredDeleteManager(std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager);
		void DrawParticle(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, Vector3 CameraPos, std::shared_ptr<Ideal::IdealCamera> Camera);
		void ComputeRenderBillboard(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);

		void SetMeshVS(std::shared_ptr<Ideal::D3D12Shader> Shader);
		void SetBillboardVS(std::shared_ptr<Ideal::D3D12Shader> Shader);
		void SetBillboardGS(std::shared_ptr<Ideal::D3D12Shader> Shader);
		void SetParticleVertexBuffer(std::shared_ptr<Ideal::D3D12VertexBuffer> ParticleVertexBuffer);

		// ��ƼŬ ��ġ ���� ��ǻƮ ����������
		void SetBillboardCalculateComputePipelineState(ComPtr<ID3D12PipelineState> PipelineState);

	private:
		void RENDER_MODE_MESH_CreatePipelineState(ComPtr<ID3D12Device> Device);
		void RENDER_MODE_BILLBOARD_CreatePipelineState(ComPtr<ID3D12Device> Device);

	private:
		ComPtr<ID3D12PipelineState> m_RENDER_MODE_MESH_pipelineState;
		ComPtr<ID3D12PipelineState> m_RENDER_MODE_BILLBOARD_pipelineState;
		ComPtr<ID3D12PipelineState> m_RENDER_MODE_BILLBOARD_ComputePipelineState;
		ComPtr<ID3D12RootSignature> m_rootSignature;
		//ComPtr<ID3D12RootSignature> m_RENDER_MODE_BILLBOARD_RootSignature;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_MESH_VS;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_BILLBOARD_VS;
		std::shared_ptr<Ideal::D3D12Shader> m_RENDER_MODE_BILLBOARD_GS;
		std::shared_ptr<Ideal::D3D12Shader> m_ps;
		std::shared_ptr<Ideal::D3D12PipelineStateObject> m_pso;

		//std::weak_ptr<Ideal::D3D12VertexBuffer> m_particleVertexBuffer;

		//----------------------------Interface---------------------------//
	public:
		virtual void SetStartColor(const DirectX::SimpleMath::Color& StartColor) override;
		virtual DirectX::SimpleMath::Color& GetStartColor() override;

		virtual void SetStartSize(float Size) override;

		virtual void SetStartLifetime(float Time) override;
		virtual float GetStartLifetime() override;

		virtual void SetSimulationSpeed(float Speed) override;
		virtual float GetSimulationSpeed() override;

		virtual void SetDuration(float Time) override;
		virtual float GetDuration() override;

		virtual void SetLoop(bool Loop) override;
		virtual bool GetLoop() override;

		//-------Emission-------//
		virtual void SetRateOverTime(bool Active) override;
		// 1�ʸ��� ������ �� �ִ� �ִ� ����
		virtual void SetEmissionRateOverTime(float Count) override;

		//------Shape------//
		virtual void SetShapeMode(bool Active) override;
		virtual void SetShape(const Ideal::ParticleMenu::EShape& Shape) override;
		virtual void SetRadius(float Radius) override;
		// 0~1������ ������ ���� �������� �β��� �����Ѵ�. �ִ� ���������� �������� �þ�� ����. 
		virtual void SetRadiusThickness(float RadiusThickness) override;

		//------Velocity Over Lifetime------//
		// ���� ��ƼŬ�� ������ ���� or ����
		virtual void SetVelocityOverLifetime(bool Active) override;
		virtual void SetVelocityDirectionMode(const Ideal::ParticleMenu::EMode& Mode) override;
		virtual void SetVelocityDirectionRandom(float Min, float Max) override;
		virtual void SetVelocityDirectionConst(const DirectX::SimpleMath::Vector3& Direction) override;
		// ���� ��ƼŬ�� �ӵ��� ���� or ����
		virtual void SetVelocitySpeedModifierMode(const Ideal::ParticleMenu::EMode& Mode) override;
		virtual void SetVelocitySpeedModifierRandom(float Min, float Max) override;
		virtual void SetVelocitySpeedModifierConst(float Speed) override;


		//------Color Over Lifetime------//
		virtual void SetColorOverLifetime(bool Active) override;
		virtual Ideal::IGradient& GetColorOverLifetimeGradientGraph() override;
		void UpdateColorOverLifetime();

		//----Size Over Lifetime----//
		virtual void SetSizeOverLifetime(bool Active) override;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisX() override;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisY() override;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisZ() override;
		void UpdateSizeOverLifetime();

		//----Rotation Over Lifetime----//
		virtual void SetRotationOverLifetime(bool Active) override;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisX() override;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisY() override;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisZ() override;

		//----------Custom Data---------//
		virtual Ideal::IBezierCurve& GetCustomData1X() override;
		virtual Ideal::IBezierCurve& GetCustomData1Y() override;
		virtual Ideal::IBezierCurve& GetCustomData1Z() override;
		virtual Ideal::IBezierCurve& GetCustomData1W() override;

		virtual Ideal::IBezierCurve& GetCustomData2X() override;
		virtual Ideal::IBezierCurve& GetCustomData2Y() override;
		virtual Ideal::IBezierCurve& GetCustomData2Z() override;
		virtual Ideal::IBezierCurve& GetCustomData2W() override;

		//-------Texture Sheet Animation-------//
		virtual void SetTextureSheetAnimation(bool Active) override;
		virtual void SetTextureSheetAnimationTiles(const DirectX::SimpleMath::Vector2& Tiles) override;

		//--------Renderer---------//
		// ���� ��带 ���� : �Ž� ���� Or ������ ��������
		virtual void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) override;
		Ideal::ParticleMenu::ERendererMode GetRenderMode();
		virtual void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) override;
		virtual void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) override;

		// ���� �� ���� �����Ͱ� �ʿ����� ���� ��� ���� ���� �Է�
		void SetCustomData(Ideal::ParticleMenu::ECustomData CustomData, Ideal::ParticleMenu::ECustomDataParameter CustomDataParameter, Ideal::ParticleMenu::ERangeMode RangeMode, float CustomDataFloat, float CustomDataFloat2 = 0.f);

	private:
		void DrawRenderMesh(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool, std::shared_ptr<Ideal::IdealCamera> Camera);
		void DrawRenderBillboard(ComPtr<ID3D12Device> Device, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12DescriptorHeap> DescriptorHeap, std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool);

		void UpdateCustomData();
		void UpdateLifeTime();

	private:
		float m_deltaTime = 0.f;
		bool m_isPlaying = true;
		bool m_stopWhenFinished = false;
		bool m_playOnWake = true;
		bool m_isActive = true;
		bool m_isLoop = true;
		float m_duration = 1.f;
		float m_currentDurationTime = 0.f;
		float m_currentTime = 0.f;
		Color m_startColor = Color(1, 1, 1, 1);
		float m_startSize = 1.f;
		float m_startLifetime = 1.f;	//1 �� �ӽ�
		float m_simulationSpeed = 1.f;

		uint32 m_maxParticles = 1;	// ��ƼŬ ����

		// System
		// �Ʒ��� üũ��. �������̽����� ���� �ٲ������ üũ��
		bool m_RENDERM_MODE_BILLBOARD_isDirty = true;
		//void SetStartPositionInCircle();

		//------Emission------//
		bool m_isUseRateOverTime = false;
		uint32 m_emissionRateOverTime = 0;

		//---------Shape---------//
		void UpdateShape();
		// ���ҽ� �Ŵ����� �̿��Ͽ� ������ ��������� ������� �ٽ� �����.
		void UpdateParticleVertexBufferAndStructuredBuffer();
		void CreateParticleStartInfo(std::vector<ComputeParticle>& Vertices);
		bool m_isUseShapeMode = false;
		Ideal::ParticleMenu::EShape m_ShapeMode_shape;
		std::shared_ptr<Ideal::D3D12StructuredBuffer> m_ParticleStructuredBuffer;
		std::shared_ptr<Ideal::D3D12VertexBuffer> m_particleVertexBuffer;
		float m_radius = 1.f;
		float m_radiusThickness = 1.f;

		//------Velocity Over Lifetime------//
		bool m_isUseVelocityOverLifetime = false;
		Ideal::ParticleMenu::EMode m_velocityDirectionMode = Ideal::ParticleMenu::EMode::Random;
		Ideal::ParticleMenu::EMode m_velocitySpeedModifierMode = Ideal::ParticleMenu::EMode::Random;
		float m_velocityRandomDirectionMin = 0.f;
		float m_velocityRandomDirectionMax = 1.f;
		Vector3 m_velocityConstDirection;
		float m_velocityRandomSpeedMin = 0.f;
		float m_velocityRandomSpeedMax = 1.f;
		float m_velocityConstSpeed;

		//------Color Over Lifetime------//
		bool m_isUseColorOverLifetime = false;
		Ideal::Gradient m_ColorOverLifetimeGradientGraph;

		//----Size Over Lifetime----//
		bool m_isSizeOverLifetime = false;
		Ideal::BezierCurve m_SizeOverLifetimeAxisX;
		Ideal::BezierCurve m_SizeOverLifetimeAxisY;
		Ideal::BezierCurve m_SizeOverLifetimeAxisZ;

		//----Rotation Over Lifetime----//
		bool m_isRotationOverLifetime = false;
		Ideal::BezierCurve m_RotationOverLifetimeAxisX;
		Ideal::BezierCurve m_RotationOverLifetimeAxisY;
		Ideal::BezierCurve m_RotationOverLifetimeAxisZ;

		//----------Custom Data---------//
		Ideal::BezierCurve m_CustomData1_X;
		Ideal::BezierCurve m_CustomData1_Y;
		Ideal::BezierCurve m_CustomData1_Z;
		Ideal::BezierCurve m_CustomData1_W;

		Ideal::BezierCurve m_CustomData2_X;
		Ideal::BezierCurve m_CustomData2_Y;
		Ideal::BezierCurve m_CustomData2_Z;
		Ideal::BezierCurve m_CustomData2_W;

		//-------Texture Sheet Animation-------//
		void UpdateAnimationUV();
		bool m_isTextureSheetAnimation;
		Vector2 m_animationTiles = Vector2(1,1);

		//------Renderer Menu------//
		bool m_RenderModeMeshBillboard = false;
		Ideal::ParticleMenu::ERendererMode m_Renderer_Mode;
		// ���� �Ʒ��� Mesh�� ERendererMode�� Mesh�� �ƴ� ��� �簢�� ������ �� ���̴�. �簢���� ����� �ϴϱ�
		std::weak_ptr<Ideal::IdealMesh<ParticleMeshVertex>> m_Renderer_Mesh;
		std::weak_ptr<Ideal::ParticleMaterial> m_particleMaterial;

		//----CB Data----//
	private:
		CB_ParticleSystem m_cbParticleSystem;
		CB_Transform m_cbTransform;
		Matrix m_transform;

	private:
		std::weak_ptr<Ideal::ResourceManager> m_ResourceManger;
		std::weak_ptr<Ideal::DeferredDeleteManager> m_DeferredDeleteManager;
	};
}