#include "Core/Core.h"
#include "IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"
#include "GraphicsEngine/Resource/IdealBone.h"
#include "GraphicsEngine/Resource/IdealAnimation.h"
#include "GraphicsEngine/Resource/IdealSkinnedMesh.h"
#include "GraphicsEngine/D3D12/D3D12Renderer.h"
#include "GraphicsEngine/D3D12/D3D12ConstantBufferPool.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/Raytracing/RaytracingManager.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorManager.h"
#include "GraphicsEngine/D3D12/D3D12UAV.h"
#include "GraphicsEngine/D3D12/Raytracing/DXRAccelerationStructureManager.h"
#include "GraphicsEngine/VertexInfo.h"
#include "GraphicsEngine/D3D12/Raytracing/RayTracingFlagManger.h"
#include <d3dx12.h>
#include <cmath>
//#include "GraphicsEngine/D3D12/D3D12DynamicConstantBufferAllocator.h"

Ideal::IdealSkinnedMeshObject::IdealSkinnedMeshObject()
{
	AddLayer(0);
}

Ideal::IdealSkinnedMeshObject::~IdealSkinnedMeshObject()
{

}

uint32 Ideal::IdealSkinnedMeshObject::GetMeshesSize()
{
	uint32 ret = m_skinnedMesh->GetMeshes().size();
	return ret;
}

std::weak_ptr<Ideal::IMesh> Ideal::IdealSkinnedMeshObject::GetMeshByIndex(uint32 index)
{
	//return m_skinnedMesh->GetMeshes()[index];
	if (index >= m_skinnedMesh->GetMeshes().size())
	{
		return std::weak_ptr<Ideal::IMesh>();
	}
	std::shared_ptr<Ideal::IdealMesh<SkinnedVertex>> mesh = m_skinnedMesh->GetMeshes()[index];
	return mesh;
}

uint32 Ideal::IdealSkinnedMeshObject::GetBonesSize()
{
	uint32 ret = m_bones.size();
	return ret;
}

std::weak_ptr<Ideal::IBone> Ideal::IdealSkinnedMeshObject::GetBoneByIndex(uint32 index)
{
	std::weak_ptr<Ideal::IdealBone> ret = m_bones[index];
	return ret;
}

void Ideal::IdealSkinnedMeshObject::Init(std::shared_ptr<IdealRenderer> Renderer)
{
	
}

void Ideal::IdealSkinnedMeshObject::Draw(std::shared_ptr<Ideal::IdealRenderer> Renderer)
{
	if (!m_isDraw)
	{
		return;
	}

	std::shared_ptr<Ideal::D3D12Renderer> d3d12Renderer = std::static_pointer_cast<Ideal::D3D12Renderer>(Renderer);
	ComPtr<ID3D12GraphicsCommandList> commandList = d3d12Renderer->GetCommandList();
	ComPtr<ID3D12Device> device = d3d12Renderer->GetDevice();
	std::shared_ptr<Ideal::D3D12DescriptorHeap> descriptorHeap = d3d12Renderer->GetMainDescriptorHeap();
	
	AnimationPlay();

	// Bind Descriptor Table
	auto handle = descriptorHeap->Allocate(2);
	uint32 incrementSize = d3d12Renderer->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	commandList->SetGraphicsRootDescriptorTable(SKINNED_MESH_DESCRIPTOR_TABLE_INDEX_OBJ, handle.GetGpuHandle());

	{
		// Bind Transform
		auto cb = d3d12Renderer->ConstantBufferAllocate(sizeof(CB_Transform));
		if (!cb)
		{
			__debugbreak();
		}

		CB_Transform* t = (CB_Transform*)cb->SystemMemAddr;
		t->World = m_transform.Transpose();
		t->WorldInvTranspose = m_transform.Transpose().Invert();

		// Copy To Main Descriptor Table
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), SKINNED_MESH_DESCRIPTOR_INDEX_CBV_TRANSFORM, incrementSize);
		device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	// Bind Bone
	{
		auto cb = d3d12Renderer->ConstantBufferAllocate(sizeof(CB_Bone));
		if (!cb)
		{
			__debugbreak();
		}

		CB_Bone* b = (CB_Bone*)cb->SystemMemAddr;
		*b = m_cbBoneData;

		// Copy To Main Descriptor Table
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvDest(handle.GetCpuHandle(), SKINNED_MESH_DESCRIPTOR_INDEX_CBV_BONE, incrementSize);
		device->CopyDescriptorsSimple(1, cbvDest, cb->CBVHandle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	m_skinnedMesh->Draw(Renderer);
}

void Ideal::IdealSkinnedMeshObject::SetAnimation(const std::string& AnimationName, bool ReserveAnimation /*= true*/)
{
	m_reservedAnimation = ReserveAnimation;
	m_nextAnimation = m_animations[AnimationName];
	if (m_nextAnimation == nullptr)
	{
		MessageBoxA(NULL, "NullException - SetAnimation", "SetAnimation", MB_OK);
		assert(false);
	}

	if (ReserveAnimation == false)
	{
		//m_cacheCurrentAnimation = m_currentAnimation;
		m_cacheCurrentAnimationFrame = m_cacheCurrentAnimationFrame;
		m_animationState = EAnimationState::ChangeAnimation;
	}
	else
	{
		//m_animationState = EAnimationState::ReserveAnimation;
	}
}

uint32 Ideal::IdealSkinnedMeshObject::GetCurrentAnimationMaxFrame()
{
	return m_currentAnimation->GetFrameCount();
}

void Ideal::IdealSkinnedMeshObject::AnimationDeltaTime(const float& DeltaTime)
{
	m_sumTime += DeltaTime;
}

void Ideal::IdealSkinnedMeshObject::AddAnimation(const std::string& AnimationName, std::shared_ptr<Ideal::IAnimation> Animation)
{
	if (m_animations[AnimationName] != nullptr)
	{
		MessageBoxA(NULL, "Already Have Same Name Animation", "AddAnimation Error", MB_OK);
		return;
	}
	m_animations[AnimationName] = std::static_pointer_cast<Ideal::IdealAnimation>(Animation);
	if (m_currentAnimation == nullptr)
	{
		m_currentAnimation = std::static_pointer_cast<Ideal::IdealAnimation>(Animation);
	}
}

void Ideal::IdealSkinnedMeshObject::SetSkinnedMesh(std::shared_ptr<Ideal::IdealSkinnedMesh> Mesh)
{
	m_skinnedMesh = Mesh;

	auto bones = Mesh->GetBones();
	for (auto b : bones)
	{
		std::shared_ptr<Ideal::IdealBone> newBone = std::make_shared<Ideal::IdealBone>();
		newBone->SetName(b->GetName());
		newBone->SetBoneIndex(b->GetBoneIndex());
		newBone->SetParent(b->GetParent());
		newBone->SetTransform(b->GetTransform());
		m_bones.push_back(newBone);
	}
}

void Ideal::IdealSkinnedMeshObject::SetStaticWhenRunTime(bool Static)
{

}

void Ideal::IdealSkinnedMeshObject::AnimationPlay()
{
	if (m_currentAnimation == nullptr)
	{
		return;
	}

	// �����ӵ��� �귯�� �ϴ� �ð�7
	float timePerFrame = 1 / (m_currentAnimation->frameRate * m_animSpeed);
	switch (m_animationState)
	{
		case EAnimationState::CurrentAnimation:
		{
			if (m_sumTime >= timePerFrame)
			{
				// ���� m_sumTime�� TimePerFrame���� ū�� ���ũ�ٸ� frame�� �׸�ŭ �����־���Ѵ�.
				uint32 shouldMoveFrameCount = m_sumTime / timePerFrame;
				m_sumTime = std::fmod(m_sumTime, timePerFrame);

				for (uint32 i = 0; i < shouldMoveFrameCount; ++i)
				{
					// ���� ������ + 1�� ���� �ִϸ��̼��� �ִ� ������ - 1 ���� Ŭ ��� �ִϸ��̼��� �����ٰ� ó���Ѵ�.
					if (m_currentFrame + 1 > m_currentAnimation->frameCount - 1)
					{
						m_isAnimationFinished = true;
					}
					m_currentFrame = (m_currentFrame + 1) % m_currentAnimation->frameCount;
					m_nextFrame = (m_currentFrame + 1) % m_currentAnimation->frameCount;
				}
			}

			m_ratio = m_sumTime / timePerFrame;

			// ��� �ִϸ��̼��� ���
			AnimationInterpolate(m_currentAnimation, m_currentFrame, m_currentAnimation, m_nextFrame);

			if (m_isAnimationFinished && m_reservedAnimation)
			{
				m_animationState = EAnimationState::ReserveAnimation;
			}
		}
		break;
		case EAnimationState::ChangeAnimation:
		{
			// ���� �ִϸ��̼ǰ� ���� �ִϸ��̼��� ���
			AnimationInterpolate(m_currentAnimation, m_cacheCurrentAnimationFrame, m_nextAnimation, 0);
			m_ratio = m_sumTime / timePerFrame;
			if (m_ratio >= 1.0f)
			{
				m_currentAnimation = m_nextAnimation;
				m_nextAnimation = nullptr;
				m_currentFrame = 0;
				m_nextFrame = 0;
				m_isAnimationFinished = false;
				m_reservedAnimation = false;
				m_animationState = EAnimationState::CurrentAnimation;
			}
		}
		break;
		case EAnimationState::ReserveAnimation:
		{
			AnimationInterpolate(m_currentAnimation, m_currentFrame, m_nextAnimation, 0);
			m_ratio = m_sumTime / timePerFrame;
			if (m_ratio >= 1.0f)
			{
				m_currentAnimation = m_nextAnimation;
				m_nextAnimation = nullptr;
				m_currentFrame = 0;
				m_nextFrame = 0;
				m_isAnimationFinished = false;
				m_reservedAnimation = false;
				m_animationState = EAnimationState::CurrentAnimation;
			}
		}
		break;
		default:
			break;

	}
	return;
}

void Ideal::IdealSkinnedMeshObject::AnimationInterpolate(std::shared_ptr<Ideal::IdealAnimation> BeforeAnimation, uint32 BeforeAnimationFrame, std::shared_ptr<Ideal::IdealAnimation> NextAnimation, uint32 NextAnimationFrame)
{
	const auto bones = m_skinnedMesh->GetBones();

	for (uint32 boneIdx = 0; boneIdx < bones.size(); ++boneIdx)
	{
		Matrix currentFrame = BeforeAnimation->m_animTransform->transforms[BeforeAnimationFrame][boneIdx];
		Matrix nextFrame = NextAnimation->m_animTransform->transforms[NextAnimationFrame][boneIdx];
		Matrix resultFrame = Matrix::Identity;
		Matrix::Lerp(currentFrame, nextFrame, m_ratio, resultFrame);
		

		Matrix finalMatrix = bones[boneIdx]->GetOffsetMatrix() * resultFrame;

		m_cbBoneData.transforms[boneIdx] = finalMatrix.Transpose();

		//if (bones[boneIdx]->GetName() == "Sword")
		//{
		//	m_cbBoneData.transforms[boneIdx] = resultFrame.Transpose();
		//}

		// SkinnedMesh"Object"�� ������ �ִ� ���� ����
		m_bones[boneIdx]->SetTransform(resultFrame);
	}
}

void Ideal::IdealSkinnedMeshObject::AlphaClippingCheck()
{
	uint32 size = m_BLASInstanceDesc->BLAS->GetGeometryDescs().size();
	auto& descs = m_BLASInstanceDesc->BLAS->GetGeometryDescs();
	for (uint32 i = 0; i < size; ++i)
	{
		if (m_skinnedMesh->GetMeshes()[i]->GetMaterial().lock()->GetIsAlphaClipping())
		{
			descs[i].Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
		}	
	}
	auto& geometries = m_BLASInstanceDesc->BLAS->GetGeometries();
	m_BLASInstanceDesc->BLAS->SetDirty(true);
}

void Ideal::IdealSkinnedMeshObject::CreateUAVVertexBuffer(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::ResourceManager> ResourceManager)
{
	auto& meshes = m_skinnedMesh->GetMeshes();
	uint64 numMesh = meshes.size();
	// �׳� �ϳ���� �����ϰ� ��
	//m_uavBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
	//m_uavBuffer->Create(Device.Get(), size, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"UAV_SkinnedVertex");
	//m_uavView = ResourceManager->CreateUAV(m_uavBuffer, numVertexCount, sizeof(BasicVertex));

	for (uint32 i = 0; i < numMesh; ++i)
	{
		uint32 numVertexCount = meshes[i]->GetVertexBuffer()->GetElementCount();
		uint32 size = numVertexCount * sizeof(BasicVertex);
		std::shared_ptr<Ideal::D3D12UAVBuffer> newVertexBuffer = std::make_shared<Ideal::D3D12UAVBuffer>();
		std::wstring name = L"UAV_SkinnedVertex";
		name += std::to_wstring(m_vertexBuffers.size());
		newVertexBuffer->Create(Device.Get(), size, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, name.c_str());
		std::shared_ptr<Ideal::D3D12UnorderedAccessView> newUAVView = ResourceManager->CreateUAV(newVertexBuffer, numVertexCount, sizeof(BasicVertex));
		m_vertexBuffers.push_back(newVertexBuffer);
		m_vertexBufferUAVs.push_back(newUAVView);
	}
}

void Ideal::IdealSkinnedMeshObject::UpdateBLASInstance(
	ComPtr<ID3D12Device5> Device,
	ComPtr<ID3D12GraphicsCommandList4> CommandList,
	std::shared_ptr<Ideal::ResourceManager> ResourceManager,
	std::shared_ptr<Ideal::D3D12DescriptorManager> DescriptorManager,
	uint32 CurrentContextIndex,
	std::shared_ptr<Ideal::D3D12DynamicConstantBufferAllocator> CBPool,
	std::shared_ptr<Ideal::RaytracingManager> RaytracingManager
)
{
	if (m_playAnimation == false)
	{
		
		return;
	}

	AnimationPlay();

	{
		auto& meshes = m_skinnedMesh->GetMeshes();
		auto& geometries = m_BLAS->GetGeometries();

		uint64 numMesh = meshes.size();

		Ideal::BLASData blasGeometryDesc;
		blasGeometryDesc.Geometries.resize(numMesh);
		for (uint32 i = 0; i < numMesh; ++i)
		{

			// Calculate Bone Transform CS
			{
				RaytracingManager->DispatchAnimationComputeShader(
					Device,
					CommandList,
					ResourceManager,
					DescriptorManager,
					CurrentContextIndex,
					CBPool,
					meshes[i]->GetVertexBuffer(),
					&m_cbBoneData,
					//m_uavView
					m_vertexBufferUAVs[i]
				);
			}
		}
		m_BLAS->SetDirty(true);
		m_BLASInstanceDesc->InstanceDesc.SetTransform(m_transform);
		//RaytracingManager->SetGeometryTransformByIndex(m_instanceIndex, m_transform);

		m_isDirty = false;
	}
}

void Ideal::IdealSkinnedMeshObject::SetBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> InBLAS)
{
	m_BLAS = InBLAS;
}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::IdealSkinnedMeshObject::GetBLAS()
{
	return m_BLAS;
}

void Ideal::IdealSkinnedMeshObject::SetBLASInstanceIndex(uint32 InstanceIndex)
{
	m_instanceIndex = InstanceIndex;
}

void Ideal::IdealSkinnedMeshObject::AddLayer(uint32 LayerNum)
{
	m_Layer.AddLayer(LayerNum);
}

void Ideal::IdealSkinnedMeshObject::DeleteLayer(uint32 LayerNum)
{
	m_Layer.DeleteLayer(LayerNum);
}

void Ideal::IdealSkinnedMeshObject::ChangeLayer(uint32 LayerNum)
{
	m_Layer.ChangeLayer(LayerNum);
}
