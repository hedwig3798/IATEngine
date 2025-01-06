#pragma once
#include <d3d12.h>
#include "GraphicsEngine/D3D12/D3D12DescriptorHeap.h"

struct ID3D12Resource;
struct ID3D12Device;

namespace Ideal
{
	class D3D12UnorderedAccessView;
	class D3D12UAVBuffer;
}
namespace Ideal
{
	class D3D12Resource
	{
	public:
		D3D12Resource();
		virtual ~D3D12Resource();

		void Release();
	public:
		ID3D12Resource* GetResource() const;
		ComPtr<ID3D12Resource> GetResourceComPtr();
	protected:
		ComPtr<ID3D12Resource> m_resource = nullptr;
	};

	// ���ε�� �ӽ� ����; ���ε� ���� ������. cpu write gpu read
	class D3D12UploadBuffer : public D3D12Resource
	{
	public:
		D3D12UploadBuffer();
		virtual ~D3D12UploadBuffer();

		// ���ε� ���۸� �����. 
		void Create(ID3D12Device* Device, uint32 BufferSize);
		void* Map();
		void* MapCpuWriteOnly();
		void UnMap();

	private:
		uint32 m_bufferSize;
	};
	
	//template <class T>
	//class D3D12StructuredBuffer : public D3D12UploadBuffer
	//{
	//public:
	//	void Create(ID3D12Device* Device, uint32 NumElements, LPCWSTR ResourceName = nulltpr)
	//	{
	//		m_staging.resize(NumElements);
	//		uint32 bufferSize = NumElements * sizeof(T);
	//		D3D12UploadBuffer::Create(Device, bufferSize);
	//		m_mappedBuffers = reinterpret_cast<T*>(D3D12UploadBuffer::Map());
	//	}
	//
	//	// ���� ���� ���ε� ���ۿ� �ø� ���� GPU�� �ø���. //
	//	void CopyStagingToGPU()
	//	{
	//		memcpy(m_mappedBuffers, &m_staging[0], m_staging.size() * sizeof(T));
	//	}
	//
	//	T& operator[](uint32 ElementIndex)
	//	{
	//		if (ElementIndex > m_staging.size() - 1)
	//		{
	//			__debugbreak();
	//			return;
	//		}
	//		return m_staging[ElementIndex];
	//	}
	//
	//	const T& operator[](uint32 ElementIndex)
	//	{
	//		if (ElementIndex > m_staging.size() - 1)
	//		{
	//			__debugbreak();
	//			return;
	//		}
	//		return m_staging[ElementIndex];
	//	}
	//
	//private:
	//	std::vector<T> m_staging;
	//	T* m_mappedBuffers;
	//};

	// GPU���� ����� ����
	class D3D12GPUBuffer : public D3D12Resource
	{
	public:
		D3D12GPUBuffer();
		virtual ~D3D12GPUBuffer();

		void SetName(const LPCTSTR& name);

	public:
		// GPU���� ����� ���۸� �����.
		// ���� �� ���ҽ� ������Ʈ�� COPY_DEST�� �ʱ�ȭ ���ش�.
		void CreateBuffer(ID3D12Device* Device, uint32 ElementSize, uint32 ElementCount, D3D12_RESOURCE_FLAGS Flags = D3D12_RESOURCE_FLAG_NONE);

		// GPU���� ���۸� �����.
		//void InitializeBuffer();
		uint32 GetBufferSize() const;
		uint32 GetElementCount() const;
		uint32 GetElementSize() const;

	protected:
		std::wstring m_name;
		uint32 m_bufferSize;
		uint32 m_elementSize;
		uint32 m_elementCount;
	};

	class D3D12StructuredBuffer : public D3D12GPUBuffer
	{
	public:
		void Create(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, uint32 ElementSize, uint32 ElementCount, const D3D12UploadBuffer& UploadBuffer);
		void Free();

		void EmplaceSRV(Ideal::D3D12DescriptorHandle SRVHandle);
		void EmplaceUAV(Ideal::D3D12DescriptorHandle UAVHandle);

		Ideal::D3D12DescriptorHandle GetSRV();
		Ideal::D3D12DescriptorHandle GetUAV();

		void TransitionToSRV(ID3D12GraphicsCommandList* CmdList);
		void TransitionToUAV(ID3D12GraphicsCommandList* CmdList);

	private:
		Ideal::D3D12DescriptorHandle m_srvHandle;
		Ideal::D3D12DescriptorHandle m_uavHandle;

	};

	// VertexBuffer
	class D3D12VertexBuffer : public D3D12GPUBuffer
	{
	public:
		D3D12VertexBuffer();
		virtual ~D3D12VertexBuffer();

	public:
		// Upload Buffer�� �ִ� �����͸� GPU Buffer�� �����Ѵ�.
		// ���ο��� ���ҽ� ����� �ɾ��ְ� Buffer View�� �����.
		void Create(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, uint32 ElementSize, uint32 ElementCount, const D3D12UploadBuffer& UploadBuffer);
		void Create(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, uint32 ElementSize, uint32 ElementCount, std::shared_ptr<Ideal::D3D12UploadBuffer> UploadBuffer);
		void CreateAndCopyResource(ComPtr<ID3D12Device> Device, uint32 ElementSize, uint32 ElementCount, ComPtr<ID3D12GraphicsCommandList> CommandList, std::shared_ptr<Ideal::D3D12Resource> Resource, D3D12_RESOURCE_STATES BeforeState);

		D3D12_VERTEX_BUFFER_VIEW GetView() const;

	private:
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	};

	// IndexBuffer
	class D3D12IndexBuffer : public D3D12GPUBuffer
	{
	public:
		D3D12IndexBuffer();
		virtual ~D3D12IndexBuffer();

	public:
		// Upload Buffer�� �ִ� �����͸� GPU Buffer�� �����Ѵ�.
		// ���ο��� ���ҽ� ����� �ɾ��ְ� Buffer View�� �����.
		void Create(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, uint32 ElementSize, uint32 ElementCount, const D3D12UploadBuffer& UploadBuffer);
		void Create(ID3D12Device* Device, ID3D12GraphicsCommandList* CmdList, uint32 ElementSize, uint32 ElementCount, std::shared_ptr<D3D12UploadBuffer> UploadBuffer);
		D3D12_INDEX_BUFFER_VIEW GetView() const;

	private:
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
	};

	// ConstantBuffer
	// ���ε� ���� ��´�. cpu write / gpu read
	// �� ������ ������ �޸��̱� ������ ������ GPU�� �־ fence/wait�� �ɾ����� �ʴ´�.
	// �� �������� �޸𸮸� ��������� �� �����Ӹ��� wait�� �ɱ� ������ �ǹ̰� �����.
	class D3D12ConstantBuffer : public D3D12Resource
	{
	public:
		D3D12ConstantBuffer();
		virtual ~D3D12ConstantBuffer();

	public:
		void Create(ID3D12Device* Device, uint32 BufferSize, uint32 FrameCount);
		// ���� ������ �ε����� �ش��ϴ� �޸��� �ּҸ� �����´�.
		void* GetMappedMemory(uint32 FrameIndex);

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress(uint32 FrameIndex);
	private:
		uint32 Align(uint32 location, uint32 align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	private:
		// �� ������ ������ : ���� * ������ ����
		uint32 m_bufferSize;

		// �� �������� ���� ������
		uint32 m_perFrameBufferSize;

		// ���۸� �Ҵ���� ������ ù �ּҸ� ����Ų��.
		void* m_mappedConstantBuffer;

		bool m_isMapped;
	};

	class D3D12UAVBuffer : public D3D12Resource
	{
		// d��// �̰� ����� �ȵƴµ� �׳� Resource�� ���߿� UAV�� �߰��ϵ� ���� �ؾ��Ѵ�.
	public:
		D3D12UAVBuffer();
		virtual ~D3D12UAVBuffer();

	public:
		void Create(ID3D12Device* Device, uint32 BufferSize, D3D12_RESOURCE_STATES InitialResourceState = D3D12_RESOURCE_STATE_COMMON, const wchar_t* Name = nullptr);
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();

		void SetUAV(std::shared_ptr<Ideal::D3D12UnorderedAccessView> UAV);
		std::shared_ptr<Ideal::D3D12UnorderedAccessView> GetUAV();
	private:
		std::shared_ptr<Ideal::D3D12UnorderedAccessView> m_uav;
	};
}