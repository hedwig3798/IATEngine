#pragma once
#include "Core/Types.h"
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"
#include "Misc/Utils/IndexCreator.h"

#include <memory>
#include <d3d12.h>
#include <d3dx12.h>

namespace Ideal
{
	// 2024.06.23
	// ��ø �����ӿ� ���Ǵ� Descriptor Pool�� ���������� 
	// ���Ǵ� CBV, SRV, UAV�� �����ϴ� Descriptor�� ������ Ŭ����
	//[----Pool----][-----Pool-----][----Fixed----]//

	// 2024.07.05
	// Fixed Pool���� �ڵ� ���� ��ȯ�Ǿ�� �Ѵ�.

	class D3D12DescriptorManager : public D3D12DescriptorHeapBase, public std::enable_shared_from_this<D3D12DescriptorManager>
	{
	public:
		D3D12DescriptorManager();
		virtual ~D3D12DescriptorManager();

		virtual EDescriptorType GetDescriptorType() override { return EDescriptorType::Static_DynamicHeap; }

	public:
		void Create(
			ComPtr<ID3D12Device> Device,
			uint32 NumDescriptorPool,
			uint32 MaxCountDescriptorPool,
			uint32 NumFixedDescriptorHeap,
			uint32 MaxCountFixedDescriptorHeap
		);

		void ResetPool(uint32 PoolIndex);
		//���̴� ���̺��� ���� �� ����ϴ� Descriptor Heap�� ��� �����͸� ���� �� ��� ������ �ٽ� �������ϴ� �׳� Reset�� �ص� �� ��?
		void ResetFixed(uint32 FixedDescriptorHeapIndex);

		Ideal::D3D12DescriptorHandle Allocate(uint32 DescriptorPoolIndex);
		Ideal::D3D12DescriptorHandle AllocateFixed(uint32 FixedDescriptorHeapIndex);

		ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() { return m_descriptorHeap; }

		void Free(int32 FixedDescriptorHeapIndex, int32 AllocatedIndex);

	private:
		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		uint32 m_maxCountDescriptorPool;
		std::vector<uint32> m_countDescriptorPool;
		uint32 m_numDescriptorPool;

		uint32 m_maxCountFixedDescriptorPool;
		std::vector<uint32> m_countFixedDescriptorPool;
		uint32 m_numFixedDescriptorPool;
		std::vector<IndexCreator> m_indexCreator;

		uint32 m_descriptorSize;
	};
}
