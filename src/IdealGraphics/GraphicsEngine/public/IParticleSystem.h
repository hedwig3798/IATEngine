#pragma once
#include <memory>
#include "../Utils/SimpleMath.h"
#include "IGraph.h"

namespace Ideal
{
	class IMesh;
	class IParticleMaterial;
}
namespace Ideal
{
	namespace ParticleMenu
	{
		// ���� ������ ���� Enum
		enum class ERangeMode
		{
			Constant = 0, // ��� 
			Curve,
			RandomBetweenConstants,	// �� ���� ��� ������ ���� ��
			RandomBetweenCurves,	// �� ���� Ŀ�� ������ ���� ��
		};

		enum class ERendererMode
		{
			Billboard,	// ī�޶� �ٶ�
			Mesh,	// �Ž� ���·� �Ž��� ���׸����� ��������.
		};

		struct ShaderData
		{
			DirectX::SimpleMath::Vector4 CustomData1;
			DirectX::SimpleMath::Vector4 CustomData2;
			float Time;
		};

		enum class ECustomData
		{
			CustomData_1 = 0,
			CustomData_2,
		};
		enum class ECustomDataParameter : unsigned int
		{
			CustomData_x = 0,
			CustomData_y,
			CustomData_z,
			CustomData_w
		};
		
		enum class EShape
		{
			Circle = 0,
		};

		enum class EMode
		{
			Const = 0,
			Random
		};
	}
}
namespace Ideal
{
	class IParticleSystem
	{
	public:
		IParticleSystem() {};
		virtual ~IParticleSystem() {};

	public:
		virtual void SetMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleMaterial) abstract;
		virtual std::weak_ptr<Ideal::IParticleMaterial> GetMaterial() abstract;

		virtual void SetTransformMatrix(const DirectX::SimpleMath::Matrix& Transform) abstract;
		virtual const DirectX::SimpleMath::Matrix& GetTransformMatrix() const abstract;

		virtual void SetActive(bool IsActive) abstract;
		virtual bool GetActive() abstract;

		virtual void SetDeltaTime(float DT) abstract;

		virtual void Play() abstract;
		virtual void Resume() abstract;	// �Ͻ� ���� ���� ��� ���� �ð����� ����
		virtual void Pause() abstract;

		virtual void SetStopWhenFinished(bool StopWhenFinished) abstract;
		virtual void SetPlayOnWake(bool PlayOnWake) abstract;

		virtual float GetCurrentDurationTime() abstract;

		virtual void SetMaxParticles(unsigned int MaxParticles) abstract;

	public:
		virtual void SetStartColor(const DirectX::SimpleMath::Color& StartColor) abstract;
		virtual DirectX::SimpleMath::Color& GetStartColor() abstract;

		virtual void SetStartSize(float Size) abstract;

		// ���� �ð�
		virtual void SetStartLifetime(float Time) abstract; // ���� ���� �ð�	( ����Ʈ�� ������ duration�� ������ �ٽ� Loop�� �ϵ� ��������)
		virtual float GetStartLifetime() abstract;

		// ��� �ӵ� ��
		virtual void SetSimulationSpeed(float Speed) abstract;
		virtual float GetSimulationSpeed() abstract;

		// �� ���� �ð�
		virtual void SetDuration(float Time) abstract;	// �� ���� �ð�
		virtual float GetDuration() abstract;

		virtual void SetLoop(bool Loop) abstract;	
		virtual bool GetLoop() abstract;

		//-------Emission-------//
		virtual void SetRateOverTime(bool Active) abstract;
		// 1�ʸ��� ������ �� �ִ� �ִ� ����
		virtual void SetEmissionRateOverTime(float Count) abstract;

		//------Shape------//
		virtual void SetShapeMode(bool Active) abstract;
		virtual void SetShape(const Ideal::ParticleMenu::EShape& Shape) abstract;
		virtual void SetRadius(float Radius) abstract;
		// 0~1������ ������ ���� �������� �β��� �����Ѵ�. �ִ� ���������� �������� �þ�� ����. 
		virtual void SetRadiusThickness(float RadiusThickness) abstract;

		//------Velocity Over Lifetime------//
		// ���� ��ƼŬ�� ������ ���� or ����
		virtual void SetVelocityOverLifetime(bool Active) abstract;
		virtual void SetVelocityDirectionMode(const Ideal::ParticleMenu::EMode& Mode) abstract;
		virtual void SetVelocityDirectionRandom(float Min, float Max) abstract;
		virtual void SetVelocityDirectionConst(const DirectX::SimpleMath::Vector3& Direction) abstract;
		// ���� ��ƼŬ�� �ӵ��� ���� or ����
		virtual void SetVelocitySpeedModifierMode(const Ideal::ParticleMenu::EMode& Mode) abstract;
		virtual void SetVelocitySpeedModifierRandom(float Min, float Max) abstract;
		virtual void SetVelocitySpeedModifierConst(float Speed) abstract;

		//------Color Over Lifetime------//
		// ���� �ð� ���� ���� ���� �ٲ����� ����. �׶���Ʈ �׷����� ���� ������
		virtual void SetColorOverLifetime(bool Active) abstract;
		virtual Ideal::IGradient& GetColorOverLifetimeGradientGraph() abstract;

		//----Rotation Over Lifetime----//
		// ���� �ð� ���� ȸ�� �� ����. ������ Ŀ�� �׷��� ���
		virtual void SetRotationOverLifetime(bool Active) abstract;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisX() abstract;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisY() abstract;
		virtual Ideal::IBezierCurve& GetRotationOverLifetimeAxisZ() abstract;

		//----Size Over Lifetime----//
		virtual void SetSizeOverLifetime(bool Active) abstract;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisX() abstract;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisY() abstract;
		virtual Ideal::IBezierCurve& GetSizeOverLifetimeAxisZ() abstract;

		//----------Custom Data---------//
		virtual Ideal::IBezierCurve& GetCustomData1X() abstract;
		virtual Ideal::IBezierCurve& GetCustomData1Y() abstract;
		virtual Ideal::IBezierCurve& GetCustomData1Z() abstract;
		virtual Ideal::IBezierCurve& GetCustomData1W() abstract;

		virtual Ideal::IBezierCurve& GetCustomData2X() abstract;
		virtual Ideal::IBezierCurve& GetCustomData2Y() abstract;
		virtual Ideal::IBezierCurve& GetCustomData2Z() abstract;
		virtual Ideal::IBezierCurve& GetCustomData2W() abstract;

		//-------Texture Sheet Animation-------//
		virtual void SetTextureSheetAnimation(bool Active) abstract;
		virtual void SetTextureSheetAnimationTiles(const DirectX::SimpleMath::Vector2& Tiles) abstract;

		//-----------Renderer-----------//
		virtual void SetRenderMode(Ideal::ParticleMenu::ERendererMode ParticleRendererMode) abstract;
		virtual void SetRenderMesh(std::shared_ptr<Ideal::IMesh> ParticleRendererMesh) abstract;
		virtual void SetRenderMaterial(std::shared_ptr<Ideal::IParticleMaterial> ParticleRendererMaterial) abstract;
	};
}