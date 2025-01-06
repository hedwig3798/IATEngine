#include "DXRAccelerationStructureManager.h"
#include "GraphicsEngine/D3D12/D3D12UploadBufferPool.h"

#include "GraphicsEngine/D3D12/D3D12RayTracingRenderer.h"
#include "GraphicsEngine/D3D12/D3D12Definitions.h"
#include "GraphicsEngine/D3D12/DeferredDeleteManager.h"

//#define OneScratchBuffer

Ideal::DXRAccelerationStructureManager::DXRAccelerationStructureManager()
{

}

Ideal::DXRAccelerationStructureManager::~DXRAccelerationStructureManager()
{

}

std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> Ideal::DXRAccelerationStructureManager::AddBLAS(std::shared_ptr<Ideal::D3D12RayTracingRenderer> Renderer, ComPtr<ID3D12Device5> Device, std::vector<BLASGeometry>& Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool AllowUpdate, const wchar_t* Name, bool IsSkinnedData /*= false*/)
{
	// ��Ű�� �������� ��� �׳� BLAS�� ���� �����.
	//if (!IsSkinnedData)
	//{
	//	// ���� ���� �̸��� BLAS�� �ִ����� �˻��Ѵ�. ���� ���� ��� �̹� �߰��� BLAS�� ��
	//	if (m_blasMap[Name] != nullptr)
	//	{
	//		//__debugbreak(); // �Ƹ� ���� �ɷ����� ���� �ڵ忡�� �̸����� ����� ã�ƿ��� ������ ���� ���̴�. 
	//		return m_blasMap[Name];
	//	}
	//}

	// ó�� �߰��� ��� ���� �־��ش�.
	std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> blas = std::make_shared<Ideal::DXRBottomLevelAccelerationStructure>(Name);
	blas->Create(Device, Geometries, BuildFlags, AllowUpdate);

	if (!IsSkinnedData)
	{
		m_blasMap[Name] = blas;
	}

	if (blas->RequiredScratchSize() > m_scratchResourceSize)
	{
		// wait�� �ɾ������ scratch buffer�� �ٽ� ������ �Ѵ�.
		Renderer->Fence();
		for (DWORD i = 0; i < Ideal::D3D12RayTracingRenderer::MAX_PENDING_FRAME_COUNT; i++)
		{
			Renderer->WaitForFenceValue(Renderer->m_lastFenceValues[i]);
		}

		m_scratchResourceSize = blas->RequiredScratchSize();

		// ver2
		for (uint32 i = 0; i < MAX_PENDING_FRAME; ++i)
		{
			m_scratchBuffers[i] = std::make_shared<Ideal::D3D12UAVBuffer>();
			m_scratchBuffers[i]->Create(Device.Get(), m_scratchResourceSize, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ASStructures2");
		}
	}
	m_blasVector.push_back(blas);
	return blas;
}

std::shared_ptr<Ideal::BLASInstanceDesc> Ideal::DXRAccelerationStructureManager::AddInstanceByBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, uint32 InstanceContributionToHitGroupIndex /*= UINT_MAX*/, Matrix transform /*= Matrix::Identity*/, BYTE InstanceMask /*= 1*/)
{
	//BLASInstanceDesc BLASInstance;
	std::shared_ptr<Ideal::BLASInstanceDesc> BLASInstance = std::make_shared<Ideal::BLASInstanceDesc>();
	BLASInstance->BLAS = BLAS;

	// ���� �ν��Ͻ��� �ε����� �����ϰ�
	//uint32 instanceIndex = m_currentBlasInstanceIndex;

	Ideal::DXRInstanceDesc instanceDesc = {};
	instanceDesc.InstanceMask = InstanceMask;

	if (InstanceContributionToHitGroupIndex == UINT_MAX)
		InstanceContributionToHitGroupIndex = BLAS->GetInstanceContributionToHitGroupIndex();

	instanceDesc.InstanceContributionToHitGroupIndex = InstanceContributionToHitGroupIndex;
	instanceDesc.AccelerationStructure = BLAS->GetGPUAddress();
	instanceDesc.SetTransform(transform);

	BLASInstance->InstanceDesc = instanceDesc;

	m_blasInstanceDescs.push_back(BLASInstance);

	return BLASInstance;
}

void Ideal::DXRAccelerationStructureManager::DeleteBLASInstance(std::shared_ptr<Ideal::BLASInstanceDesc> Instance)
{
	auto it = std::find(m_blasInstanceDescs.begin(), m_blasInstanceDescs.end(), Instance);
	if (it != m_blasInstanceDescs.end())
	{
		*it = std::move(m_blasInstanceDescs.back());
		//std::swap(*it, m_blasInstanceDescs.back());
		m_blasInstanceDescs.pop_back();
	}
}

void Ideal::DXRAccelerationStructureManager::DeleteBLAS(std::shared_ptr<Ideal::DXRBottomLevelAccelerationStructure> BLAS, const std::wstring& Name, bool IsSkinnedData)
{ 
	if (!IsSkinnedData)
	{
		auto it = m_blasMap.find(Name);
		if (it != m_blasMap.end())
		{
			m_blasMap.erase(it);
		}
	}
	auto it = std::find(m_blasVector.begin(), m_blasVector.end(), BLAS);
	if (it != m_blasVector.end())
	{
		*it = std::move(m_blasVector.back());
		m_blasVector.pop_back();
	}
}

void Ideal::DXRAccelerationStructureManager::InitTLAS(ComPtr<ID3D12Device5> Device, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool allowUpdate /*= false*/, const wchar_t* TLASName /*= nullptr*/)
{
	for (uint32 i = 0; i < MAX_PENDING_FRAME; ++i)
	{
		m_topLevelASs[i] = std::make_shared<Ideal::DXRTopLevelAccelerationStructure>(TLASName);
		m_topLevelASs[i]->Create(Device, m_blasInstanceDescs.size(), BuildFlags, nullptr, allowUpdate);
	}

	m_scratchResourceSize = max(m_topLevelASs[0]->RequiredScratchSize(), m_scratchResourceSize);

	for (uint32 i = 0; i < MAX_PENDING_FRAME; ++i)
	{
		m_scratchBuffers[i] = std::make_shared<Ideal::D3D12UAVBuffer>();
		m_scratchBuffers[i]->Create(Device.Get(), m_scratchResourceSize, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ASStructures2");
	}
}

void Ideal::DXRAccelerationStructureManager::Build(ComPtr<ID3D12Device5> Device, ComPtr<ID3D12GraphicsCommandList4> CommandList, std::shared_ptr<Ideal::D3D12UploadBufferPool> UploadBufferPool, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager, bool ForceBuild /*= false*/)
{
	UploadBufferPool->Reset();
	m_currentIndex = (m_currentIndex + 1) % MAX_PENDING_FRAME;

	// instanceDescs�� ù �ּҸ� �޾ƿ´�.
	D3D12_GPU_VIRTUAL_ADDRESS instanceDescs = 0;
	bool isFirst = true;

	// instance ������ UpdateBuffer�� �ø���.
	uint32 numInstance = m_blasInstanceDescs.size();

	for (uint32 i = 0; i < numInstance; ++i)
	{
		std::shared_ptr<Ideal::UploadBufferContainer> container = UploadBufferPool->Allocate();
		if (isFirst)
		{
			instanceDescs = container->GpuVirtualAddress;
			isFirst = false;
		}

		DXRInstanceDesc* ptr = (DXRInstanceDesc*)container->SystemMemoryAddress;
		*ptr = m_blasInstanceDescs[i]->InstanceDesc;

		// 2024.08.20 ContributionToHitGroupIndex�� ������ �ٽ� ���ش�.	// �̰ŋ��� GPU �޸� �߸� �����ϴ� ������ �������. �̰� ã�� ���� �������.
		ptr->InstanceContributionToHitGroupIndex = m_blasInstanceDescs[i]->BLAS->GetInstanceContributionToHitGroupIndex();	
	}

	// Build BLAS
	for (auto& blas : m_blasVector)
	{
		// ó���� �����ؾ��ϰų� blas ������ �����Ǿ� �ٽ� �����ؾ� �� ���
		if (ForceBuild || blas->IsDirty())
		{
			// ver2
			blas->Build(CommandList, m_scratchBuffers[m_currentIndex]->GetResource());
			CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(blas->GetResource().Get());
			CommandList->ResourceBarrier(1, &uavBarrier);
		}
	}

	// Build TLAS
	{
		DeferredDeleteManager->AddTLASToDeferredDelete(m_topLevelASs[m_currentIndex]);

		//m_topLevelASs[m_currentIndex]->Create(Device, m_blasInstanceDescs.size(), D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, nullptr, false);
		m_topLevelASs[m_currentIndex] = std::make_shared<Ideal::DXRTopLevelAccelerationStructure>(L"TLAS");
		m_topLevelASs[m_currentIndex]->Create(Device, m_blasInstanceDescs.size(), D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE, DeferredDeleteManager, false);
		m_topLevelASs[m_currentIndex]->Build(CommandList, numInstance, instanceDescs, m_scratchBuffers[m_currentIndex]->GetResource());

		CD3DX12_RESOURCE_BARRIER uavBarrier = CD3DX12_RESOURCE_BARRIER::UAV(m_topLevelASs[m_currentIndex]->GetResource().Get());
		CommandList->ResourceBarrier(1, &uavBarrier);
	}
}