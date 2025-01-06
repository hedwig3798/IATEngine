#include "Core/Core.h"

//#include "GraphicsEngine/D3D12/D3D12ThirdParty.h"
#include <d3d12.h>
#include <d3dx12.h>
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/ResourceManager.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/D3D12Texture.h"
#include "GraphicsEngine/D3D12/D3D12SRV.h"
#include "GraphicsEngine/D3D12/D3D12UAV.h"
#include "GraphicsEngine/D3D12/Raytracing/RaytracingManager.h"


#include "GraphicsEngine/Resource/ShaderManager.h"
#include "GraphicsEngine/D3D12/D3D12Shader.h"

#include "GraphicsEngine/Resource/IdealStaticMesh.h"
#include "GraphicsEngine/Resource/IdealStaticMeshObject.h"
#include "GraphicsEngine/Resource/IdealSkinnedMesh.h"
#include "GraphicsEngine/Resource/IdealSkinnedMeshObject.h"
#include "GraphicsEngine/Resource/IdealBone.h"
#include "GraphicsEngine/Resource/IdealMesh.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"
#include "GraphicsEngine/Resource/IdealAnimation.h"
#include "GraphicsEngine/Resource/ModelAnimation.h"

#include "GraphicsEngine/D3D12/DeferredDeleteManager.h"

#include "ThirdParty/Include/DirectXTK12/WICTextureLoader.h"
#include "ThirdParty/Include/DirectXTK12/DDSTextureLoader.h"
#include "ThirdParty/Include/DirectXTex/DirectXTex.h"
#include "Misc/Utils/FileUtils.h"
#include "Misc/Utils/StringUtils.h"
#include "Misc/Utils/tinyxml2.h"

#include <filesystem>

using namespace Ideal;

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
	Fence();
	WaitForFenceValue();

	m_defaultMaterial.reset();
	m_defaultAlbedo.reset();
	m_defaultNormal.reset();
	m_defaultMask.reset();
	m_textures.clear();

	//m_deferredDeleteManager->AddTextureToDeferredDelete(m_defaultAlbedo);
	//m_deferredDeleteManager->AddTextureToDeferredDelete(m_defaultNormal);
	//m_deferredDeleteManager->AddTextureToDeferredDelete(m_defaultMask);
}

void Ideal::ResourceManager::Init(ComPtr<ID3D12Device5> Device, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager)
{
	m_device = Device;
	m_deferredDeleteManager = DeferredDeleteManager;

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	Check(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf())));

	//-----------Create Command Allocator---------//
	Check(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_commandAllocator.GetAddressOf())));
	m_commandAllocator->SetName(L"ResourceAllocator");

	//-----------Create Command List---------//
	Check(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.GetAddressOf())));
	m_commandList->Close();

	//-----------Create Fence---------//
	Check(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf())));
	m_fenceValue = 0;
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	m_fence->SetName(L"ResourceManager Fence");

	//------------SRV Heap-----------//
	m_cbv_srv_uavHeap = std::make_shared<D3D12DynamicDescriptorHeap>();
	m_cbv_srv_uavHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, m_srvHeapCount);
	//m_srvHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, m_srvHeapCount);

	//------------RTV Heap-----------//
	m_rtvHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
	//m_rtvHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);
	m_rtvHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, MAX_RTV_HEAP_COUNT);

	//-----------DSV Heap------------//
	m_dsvHeap = std::make_shared<Ideal::D3D12DynamicDescriptorHeap>();
	m_dsvHeap->Create(m_device.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, MAX_DSV_HEAP_COUNT);

	//--------Manager--------//
	m_uploadCommandListPoolManager = std::make_shared<Ideal::UploadCommandListPool>();
	m_uploadCommandListPoolManager->Init(Device, m_fence, UPLOAD_CONTAINER_COUNT);

	//----default resource----//
	CreateDefaultQuadMesh();
	CreateDefaultQuadMesh2();
	CreateDefaultDebugLine();
	CreateParticleVertexBuffer();
}

void ResourceManager::WaitForResourceUpload()
{
	m_uploadCommandListPoolManager->AllWaitForFenceValue();
}

void ResourceManager::Fence()
{
	m_fenceValue++;
	m_commandQueue->Signal(m_fence.Get(), m_fenceValue);

	//uint64 fenceValue = m_fence->GetCompletedValue();
	//std::string result = "Current Fence :" + std::to_string(fenceValue) + "\n";
	//std::string result2 = "Current Fence Value : " + std::to_string(m_fenceValue) + "\n";
	//OutputDebugStringA(result.c_str());
	//OutputDebugStringA(result2.c_str());
}

void ResourceManager::WaitForFenceValue()
{
	const uint64 expectedFenceValue = m_fenceValue;
	//std::string count = std::to_string(m_fence->GetCompletedValue());
	//std::string count2 = std::to_string(expectedFenceValue);
	//std::string result = "resource manager" + count + " : " + count2 + "\n";
	//OutputDebugStringA(result.c_str());

	if (m_fence->GetCompletedValue() < expectedFenceValue)
	{
		m_fence->SetEventOnCompletion(expectedFenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

void ResourceManager::CreateVertexBufferBox(std::shared_ptr<Ideal::D3D12VertexBuffer>& VertexBuffer)
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	ComPtr<ID3D12Resource> vertexBufferUpload = nullptr;	// ���ε� ������ ���� �ϳ��� ����� �� ����.
	const VertexTest2 cubeVertices[] = {
		{ { -0.5f, 0.5f, -0.5f},	Vector2(0.0f, 1.0f) },    // Back Top Left
		{ {  0.5f, 0.5f, -0.5f},	Vector2(0.0f, 0.0f) },    // Back Top Right
		{ {  0.5f, 0.5f,  0.5f},	Vector2(1.0f, 0.0f) },    // Front Top Right
		{ { -0.5f, 0.5f,  0.5f},	Vector2(1.0f, 1.0f)	},    // Front Top Left

		{ { -0.5f, -0.5f,-0.5f},	Vector2(0.0f, 1.0f)	},    // Back Bottom Left
		{ {  0.5f, -0.5f,-0.5f},	Vector2(0.0f, 0.0f) },    // Back Bottom Right
		{ {  0.5f, -0.5f, 0.5f},	Vector2(1.0f, 0.0f) },    // Front Bottom Right
		{ { -0.5f, -0.5f, 0.5f},	Vector2(1.0f, 1.0f) },    // Front Bottom Left
	};
	const uint32 vertexBufferSize = sizeof(cubeVertices);

	// UploadBuffer�� �����.
	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), vertexBufferSize);

	void* mappedUploadHeap = uploadBuffer.Map();
	memcpy(mappedUploadHeap, cubeVertices, sizeof(cubeVertices));
	uploadBuffer.UnMap();

	uint32 vertexTypeSize = sizeof(VertexTest2);
	uint32 vertexCount = _countof(cubeVertices);
	VertexBuffer->Create(m_device.Get(), m_commandList.Get(), vertexTypeSize, vertexCount, uploadBuffer);

	// close
	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	VertexBuffer->SetName(L"TestBoxVB");

	Fence();
	WaitForFenceValue();
}

void ResourceManager::CreateIndexBufferBox(std::shared_ptr<Ideal::D3D12IndexBuffer>& IndexBuffer)
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	uint32 indices[] = {
			0, 1, 3,
			1, 2, 3,

			3, 2, 7,
			6, 7, 2,

			2, 1, 6,
			5, 6, 1,

			1, 0, 5,
			4, 5, 0,

			0, 3, 4,
			7, 4, 3,

			7, 6, 4,
			5, 4, 6, };
	const uint32 indexBufferSize = sizeof(indices);

	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), indexBufferSize);


	// ���ε���ۿ� ���� ����
	void* mappedUploadBuffer = uploadBuffer.Map();
	memcpy(mappedUploadBuffer, indices, indexBufferSize);
	uploadBuffer.UnMap();

	uint32 indexTypeSize = sizeof(uint32);
	uint32 indexCount = _countof(indices);

	IndexBuffer->Create(m_device.Get(), m_commandList.Get(), indexTypeSize, indexCount, uploadBuffer);

	//-----------Execute-----------//
	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	IndexBuffer->SetName(L"TestBoxIB");

	Fence();
	WaitForFenceValue();
}

void ResourceManager::CreateIndexBuffer(std::shared_ptr<Ideal::D3D12IndexBuffer> OutIndexBuffer, std::vector<uint32>& Indices)
{
#ifndef USE_UPLOAD_CONTAINER
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	const uint32 elementSize = sizeof(uint32);
	const uint32 elementCount = (uint32)Indices.size();
	const uint32 bufferSize = elementSize * elementCount;

	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), bufferSize);
	{
		uploadBuffer.Map();
		void* mappedData = uploadBuffer.Map();
		memcpy(mappedData, Indices.data(), bufferSize);
		uploadBuffer.UnMap();
	}
	OutIndexBuffer->Create(m_device.Get(),
		m_commandList.Get(),
		elementSize,
		elementCount,
		uploadBuffer
	);

	//-----------Execute-----------//
	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	Fence();
	WaitForFenceValue();
#else
	const uint32 elementSize = sizeof(uint32);
	const uint32 elementCount = (uint32)Indices.size();
	const uint32 bufferSize = elementSize * elementCount;

	auto Container = m_uploadCommandListPoolManager->AllocateUploadContainer(bufferSize);

	{
		void* mappedData = Container->UploadBuffer->Map();
		memcpy(mappedData, Indices.data(), bufferSize);
		Container->UploadBuffer->UnMap();
	}
	OutIndexBuffer->Create(m_device.Get(),
		Container->CommandList.Get(),
		elementSize,
		elementCount,
		Container->UploadBuffer
	);

	//-----------Execute-----------//
	Container->CloseAndExecute(m_commandQueue, m_fence);
	Fence();
	Container->FenceValue = m_fenceValue;
#endif
}

void ResourceManager::CreateIndexBufferUint16(std::shared_ptr<Ideal::D3D12IndexBuffer> OutIndexBuffer, std::vector<uint16>& Indices)
{
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	const uint32 elementSize = sizeof(uint16);
	const uint32 elementCount = (uint32)Indices.size();
	const uint32 bufferSize = elementSize * elementCount;

	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), bufferSize);
	{
		uploadBuffer.Map();
		void* mappedData = uploadBuffer.Map();
		memcpy(mappedData, Indices.data(), bufferSize);
		uploadBuffer.UnMap();
	}
	OutIndexBuffer->Create(m_device.Get(),
		m_commandList.Get(),
		elementSize,
		elementCount,
		uploadBuffer
	);

	//-----------Execute-----------//
	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	Fence();
	WaitForFenceValue();
}

uint64 ResourceManager::AllocateMaterialID()
{
	uint64 ret = m_materialID;
	m_materialID++;
	return ret;
}

void Ideal::ResourceManager::CreateTexture(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, const std::wstring& Path, bool IgnoreSRGB /*= false*/, uint32 MipLevels /*= 1*/, bool IsNormalMap /*= false*/)
{

	std::string name = StringUtils::ConvertWStringToString(Path);
	if (m_textures[name] != nullptr)
	{
		OutTexture = m_textures[name];
		return;
	}

	m_textures[name] = std::make_shared<Ideal::D3D12Texture>();
	OutTexture = m_textures[name];
	OutTexture->SetName(name);
	//if (!OutTexture)
	//{
	//	OutTexture = std::make_shared<Ideal::D3D12Texture>();
	//}
	// 2024.05.14 : �� �Լ� Texture�� �־�� ������// 
	// 2024.05.15 : fence������ �� �𸣰ڴ�. �ϴ� �Ѿ��.

	//uint32 MipLevelsCount = MipLevels;


	/// TGA üũ ///
	//std::filesystem::path path = name;
	bool isTGA = false;
	if (name.compare(name.size() - 4, 4, ".tga") == 0 || name.compare(name.size() - 4, 4, ".TGA") == 0)
	{
		isTGA = true;
	}

	//----------------------Init--------------------------//
#ifndef USE_UPLOAD_CONTAINER
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);
#endif
	ComPtr<ID3D12Resource> resource;
	Ideal::D3D12DescriptorHandle srvHandle;
	//D3D12_SUBRESOURCE_DATA subResource;
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;


	D3D12_RESOURCE_FLAGS resourceFlag = D3D12_RESOURCE_FLAG_NONE;

	//----------------------Load TGA Texture From File---------------------//
	DirectX::ScratchImage image;
	DirectX::TexMetadata metadata;
	const DirectX::Image* img = nullptr;
	if (isTGA)
	{
		//Check(DirectX::LoadFromTGAFile(Path.c_str(), nullptr, image), L"Failed To Load TGA File");
		Check(DirectX::LoadFromTGAFile(Path.c_str(), &metadata, image), L"Failed To Load TGA File");
		img = image.GetImages();
	}
	else
	{
		Check(DirectX::LoadFromWICFile(Path.c_str(), WIC_FLAGS_FORCE_RGB, &metadata, image), L"Failed to load WIC from file");
		img = image.GetImages();
	}
	
	if (IgnoreSRGB || IsNormalMap)
	{
		metadata.format = MakeLinear(metadata.format);
	}

	MipLevels = (MipLevels == 0) ?
		1 + static_cast<UINT16>(std::floor(std::log2(std::max<uint64>(metadata.width, metadata.height)))) :
		static_cast<UINT16>(MipLevels);

	// --------------------- MIP �� ���� (GenerateMipMaps ���) ------------------------//
	DirectX::ScratchImage mipChain;
	if (MipLevels > 1)
	{
		//Check(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, MipLevels, mipChain), L"Failed to generate MIP maps");
		//Check(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_LINEAR, MipLevels, mipChain), L"Failed to generate MIP maps");
		if (IsNormalMap)
		{
			Check(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT | DirectX::TEX_FILTER_FORCE_NON_WIC, MipLevels, mipChain), L"Failed to generate MIP maps");
			//Check(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT | DirectX::TEX_FILTER_LINEAR, MipLevels, mipChain), L"Failed to generate MIP maps");
		}
		else
		{
			//Check(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_FANT, MipLevels, mipChain), L"Failed to generate MIP maps");
			Check(DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_DEFAULT, MipLevels, mipChain), L"Failed to generate MIP maps");
		}
		
		img = mipChain.GetImages();
	}
	// �� MIP ������ ���긮�ҽ� ������ ����
	for (uint32_t i = 0; i < MipLevels; ++i) {
		D3D12_SUBRESOURCE_DATA subResource = {};
		subResource.pData = img[i].pixels;
		subResource.RowPitch = img[i].rowPitch;
		subResource.SlicePitch = img[i].slicePitch;
		subResources.push_back(subResource);
	}
	// �ؽ�ó ���ҽ� ����
	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		(uint64)metadata.width,
		(uint32)metadata.height,
		(uint16)metadata.arraySize,
		MipLevels // �ڵ� ������ MIP�� ���� �� �ݿ�
	);

	Check(m_device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(resource.GetAddressOf())
	));

	//----------------------Upload Buffer--------------------------//

	uint64 bufferSize = GetRequiredIntermediateSize(resource.Get(), 0, MipLevels);

#ifndef USE_UPLOAD_CONTAINER
	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), (uint32)bufferSize);
#else
	auto uploadContainer = m_uploadCommandListPoolManager->AllocateUploadContainer(bufferSize);
	auto uploadBuffer = uploadContainer->UploadBuffer;
#endif
	//----------------------Update Subresources--------------------------//
#ifndef USE_UPLOAD_CONTAINER
	//uploadBuffer.Map();
	UpdateSubresources(
		m_commandList.Get(),
		resource.Get(),
		uploadBuffer.GetResource(),
		0, 0, static_cast<UINT>(subResources.size()), subResources.data()
	);
	
	//uploadBuffer.UnMap();
#else
	//uploadBuffer->Map();
	UpdateSubresources(
		uploadContainer->CommandList.Get(),
		resource.Get(),
		uploadBuffer->GetResource(),
		0, 0, static_cast<UINT>(subResources.size()), subResources.data()
	);
	//uploadBuffer->UnMap();
#endif
	//----------------------Resource Barrier--------------------------//

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
#ifndef USE_UPLOAD_CONTAINER
	m_commandList->ResourceBarrier(1, &barrier);
#else
	uploadContainer->CommandList->ResourceBarrier(1, &barrier);
#endif
	//----------------------Execute--------------------------//

#ifndef USE_UPLOAD_CONTAINER
	m_commandList->Close();
	
	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	Fence();

	WaitForFenceValue();
#else
	uploadContainer->CloseAndExecute(m_commandQueue, m_fence);
	Fence();
	uploadContainer->FenceValue = m_fenceValue;
#endif
	//WaitForFenceValue();

	//----------------------Create Shader Resource View--------------------------//

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resource->GetDesc().Format;
	//if (IgnoreSRGB)
	//{
	//	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//}
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = MipLevels;
	srvHandle = m_cbv_srv_uavHeap->Allocate();
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = srvHandle.GetCpuHandle();
	m_device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);

	//----------------------Create Unordered Access View--------------------------//
	//{
	//	for (uint32 i = 0; i < GenerateMips; i++)
	//	{
	//		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	//		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	//		uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//		uavDesc.Texture2D.MipSlice = i;
	//		uavDesc.Texture2D.PlaneSlice = 0;
	//
	//		auto handle = m_cbv_srv_uavHeap->Allocate();
	//
	//		m_device->CreateUnorderedAccessView(resource.Get(), nullptr, &uavDesc, handle.GetCpuHandle());
	//		OutTexture->EmplaceUAV(handle);
	//	}
	//}

	//----------------------Final Create---------------------//
	OutTexture->Create(resource, m_deferredDeleteManager);
	OutTexture->EmplaceSRV(srvHandle);
}

void ResourceManager::CreateDynamicTexture(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, uint32 Width, uint32 Height)
{
	ComPtr<ID3D12Resource> TextureResource;
	ComPtr<ID3D12Resource> UploadBuffer;

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ex) DXGI_FORMAT_R8G8B8A8_UNORM, etc...
	textureDesc.Width = Width;
	textureDesc.Height = Height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;


	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	Check(m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(TextureResource.GetAddressOf())
	));

	uint64 uploadBufferSize = GetRequiredIntermediateSize(TextureResource.Get(), 0, 1);

	CD3DX12_HEAP_PROPERTIES heapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
	Check(m_device->CreateCommittedResource(
		&heapPropUpload,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(UploadBuffer.GetAddressOf())
	));

	//BYTE* data;
	//UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&data));
	//ZeroMemory(data, static_cast<size_t>(uploadBufferSize));
	//UploadBuffer->Unmap(0, nullptr);

	OutTexture = std::make_shared<Ideal::D3D12Texture>();
	OutTexture->Create(TextureResource, m_deferredDeleteManager);
	OutTexture->SetUploadBuffer(UploadBuffer, uploadBufferSize);

	// srv
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	auto handle = m_cbv_srv_uavHeap->Allocate();
	m_device->CreateShaderResourceView(TextureResource.Get(), &srvDesc, handle.GetCpuHandle());

	OutTexture->EmplaceSRV(handle);
}

void ResourceManager::CreateTextureDDS(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, const std::wstring& Path)
{
	if (!OutTexture)
	{
		OutTexture = std::make_shared<Ideal::D3D12Texture>();
	}
#ifndef USE_UPLOAD_CONTAINER
	// 2024.05.14 : �� �Լ� Texture�� �־�� ������// 
	// 2024.05.15 : fence������ �� �𸣰ڴ�. �ϴ� �Ѿ��.

	//----------------------Init--------------------------//
	m_commandAllocator->Reset();
	m_commandList->Reset(m_commandAllocator.Get(), nullptr);

	ComPtr<ID3D12Resource> resource;
	Ideal::D3D12DescriptorHandle srvHandle;

	//----------------------Load DDS Texture From File--------------------------//

	std::unique_ptr<uint8_t[]> decodeData;
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;

	Check(DirectX::LoadDDSTextureFromFile(
		m_device.Get(),
		Path.c_str(),
		resource.ReleaseAndGetAddressOf(),
		decodeData,
		subResources
	));

	//----------------------Upload Buffer--------------------------//

	uint64 bufferSize = GetRequiredIntermediateSize(resource.Get(), 0, static_cast<uint32>(subResources.size()));

	Ideal::D3D12UploadBuffer uploadBuffer;
	uploadBuffer.Create(m_device.Get(), (uint32)bufferSize);

	//----------------------Update Subresources--------------------------//
	uploadBuffer.Map();
	UpdateSubresources(
		m_commandList.Get(),
		resource.Get(),
		uploadBuffer.GetResource(),
		0, 0, subResources.size(), subResources.data()
	);

	uploadBuffer.UnMap();
	//----------------------Resource Barrier--------------------------//

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	m_commandList->ResourceBarrier(1, &barrier);

	//----------------------Execute--------------------------//

	m_commandList->Close();

	ID3D12CommandList* commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	Fence();
	WaitForFenceValue();
#endif

#ifdef USE_UPLOAD_CONTAINER
	ComPtr<ID3D12Resource> resource;
	Ideal::D3D12DescriptorHandle srvHandle;

	//----------------------Load DDS Texture From File--------------------------//

	std::unique_ptr<uint8_t[]> decodeData;
	std::vector<D3D12_SUBRESOURCE_DATA> subResources;

	Check(DirectX::LoadDDSTextureFromFile(
		m_device.Get(),
		Path.c_str(),
		resource.ReleaseAndGetAddressOf(),
		decodeData,
		subResources
	));

	//----------------------Upload Buffer--------------------------//

	uint64 bufferSize = GetRequiredIntermediateSize(resource.Get(), 0, static_cast<uint32>(subResources.size()));

	auto Container = m_uploadCommandListPoolManager->AllocateUploadContainer(bufferSize);

	//----------------------Update Subresources--------------------------//
	Container->UploadBuffer->Map();

	UpdateSubresources(
		Container->CommandList.Get(),
		resource.Get(),
		Container->UploadBuffer->GetResource(),
		0, 0, subResources.size(), subResources.data()
	);

	Container->UploadBuffer->UnMap();
	//----------------------Resource Barrier--------------------------//

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		resource.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	Container->CommandList->ResourceBarrier(1, &barrier);

	//----------------------Execute--------------------------//

	Container->CloseAndExecute(m_commandQueue, m_fence);
	Fence();
	Container->FenceValue = m_fenceValue;
#endif



	//----------------------Create Shader Resource View--------------------------//

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resource->GetDesc().Format;
	srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	//----------------------Allocate Descriptor-----------------------//
	srvHandle = m_cbv_srv_uavHeap->Allocate();
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = srvHandle.GetCpuHandle();
	m_device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);

	//----------------------Final Create---------------------//
	OutTexture->Create(resource, m_deferredDeleteManager);
	OutTexture->EmplaceSRV(srvHandle);


}

void ResourceManager::CreateEmptyTexture2D(std::shared_ptr<Ideal::D3D12Texture>& OutTexture, const uint32& Width, const uint32 Height, DXGI_FORMAT Format, const Ideal::IdealTextureTypeFlag& TextureFlags, const std::wstring& Name)
{
	DXGI_FORMAT MakeFormat = Format;
	bool makeRTV = false;
	bool makeSRV = false;
	bool makeUAV = false;
	bool makeDSV = false;
	if (TextureFlags & Ideal::IDEAL_TEXTURE_RTV)
		makeRTV = true;
	if (TextureFlags & Ideal::IDEAL_TEXTURE_SRV)
		makeSRV = true;
	if (TextureFlags & Ideal::IDEAL_TEXTURE_UAV)
		makeUAV = true;
	if (TextureFlags & Ideal::IDEAL_TEXTURE_DSV)
	{
		makeRTV = false;
		makeSRV = false;
		makeUAV = false;
		makeDSV = true;
		MakeFormat = DXGI_FORMAT_D32_FLOAT;
	}

	if (!OutTexture)
	{
		OutTexture = std::make_shared<Ideal::D3D12Texture>();
	}

	ComPtr<ID3D12Resource> resource;
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		MakeFormat,
		Width,
		Height,
		1, 1, 1, 0
	);

	if (makeRTV)
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	if (makeUAV)
		resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	if (makeDSV)
		resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	Check(m_device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		//D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,	// ClearValue
		IID_PPV_ARGS(resource.GetAddressOf())
	));

	resource->SetName(Name.c_str());
	OutTexture->Create(resource, m_deferredDeleteManager);

	if (makeSRV)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		Ideal::D3D12DescriptorHandle srvHandle = m_cbv_srv_uavHeap->Allocate();
		m_device->CreateShaderResourceView(resource.Get(), &srvDesc, srvHandle.GetCpuHandle());

		OutTexture->EmplaceSRV(srvHandle);
	}

	if (makeRTV)
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = Format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		Ideal::D3D12DescriptorHandle rtvHandle = m_rtvHeap->Allocate();
		m_device->CreateRenderTargetView(resource.Get(), &rtvDesc, rtvHandle.GetCpuHandle());

		OutTexture->EmplaceRTV(rtvHandle);
	}

	if (makeUAV)
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		Ideal::D3D12DescriptorHandle uavHandle = m_cbv_srv_uavHeap->Allocate();
		m_device->CreateUnorderedAccessView(resource.Get(), nullptr, &uavDesc, uavHandle.GetCpuHandle());

		OutTexture->EmplaceUAV(uavHandle);
	}

	if (makeDSV)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		auto dsvHandle = m_dsvHeap->Allocate();
		m_device->CreateDepthStencilView(resource.Get(), &dsvDesc, dsvHandle.GetCpuHandle());

		OutTexture->EmplaceDSV(dsvHandle);
	}
}

std::shared_ptr<Ideal::D3D12ShaderResourceView> ResourceManager::CreateSRV(std::shared_ptr<Ideal::D3D12Resource> Resource, uint32 NumElements, uint32 ElementSize)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = NumElements;
	if (ElementSize == 0)
	{
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		srvDesc.Buffer.StructureByteStride = 0;
	}
	else
	{
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.StructureByteStride = ElementSize;
	}

	Ideal::D3D12DescriptorHandle allocatedHandle = m_cbv_srv_uavHeap->Allocate();
	m_device->CreateShaderResourceView(Resource->GetResource(), &srvDesc, allocatedHandle.GetCpuHandle());
	std::shared_ptr<Ideal::D3D12ShaderResourceView> ret = std::make_shared<Ideal::D3D12ShaderResourceView>();
	ret->SetResourceLocation(allocatedHandle);
	return ret;
}

std::shared_ptr<Ideal::D3D12UnorderedAccessView> ResourceManager::CreateUAV(std::shared_ptr<Ideal::D3D12Resource> Resource, uint32 NumElements, uint32 ElementSize)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.NumElements = NumElements;
	uavDesc.Buffer.StructureByteStride = ElementSize;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

	Ideal::D3D12DescriptorHandle allocatedHandle = m_cbv_srv_uavHeap->Allocate();
	m_device->CreateUnorderedAccessView(Resource->GetResource(), nullptr, &uavDesc, allocatedHandle.GetCpuHandle());
	std::shared_ptr<Ideal::D3D12UnorderedAccessView> ret = std::make_shared<Ideal::D3D12UnorderedAccessView>();
	ret->SetResourceLocation(allocatedHandle);
	ret->SetResource(Resource->GetResource());
	return ret;
}

std::shared_ptr<Ideal::D3D12ShaderResourceView> ResourceManager::CreateSRV(ComPtr<ID3D12Resource> Resource, uint32 NumElements, uint32 ElementSize)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = NumElements;
	if (ElementSize == 0)
	{
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
		srvDesc.Buffer.StructureByteStride = 0;
	}
	else
	{
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.StructureByteStride = ElementSize;
	}

	Ideal::D3D12DescriptorHandle allocatedHandle = m_cbv_srv_uavHeap->Allocate();
	m_device->CreateShaderResourceView(Resource.Get(), &srvDesc, allocatedHandle.GetCpuHandle());
	std::shared_ptr<Ideal::D3D12ShaderResourceView> ret = std::make_shared<Ideal::D3D12ShaderResourceView>();
	ret->SetResourceLocation(allocatedHandle);
	return ret;
}

void Ideal::ResourceManager::CreateStaticMeshObject(std::shared_ptr<Ideal::IdealStaticMeshObject> OutMesh, const std::wstring& filename, bool IsDebugMesh /*= false*/)
{
	// �̹� ���� ���
	std::string key = StringUtils::ConvertWStringToString(filename);

	std::shared_ptr<Ideal::IdealStaticMesh> staticMesh;
	if (!IsDebugMesh)
	{
		staticMesh = m_staticMeshes[key];
		if (staticMesh != nullptr)
		{
			staticMesh->AddRefCount();
			OutMesh->SetStaticMesh(staticMesh);
			return;
		}
	}

	staticMesh = std::make_shared<Ideal::IdealStaticMesh>();
	staticMesh->AddRefCount();
	// ������ StaticMesh�� ���� ������ �־��ָ�ȴ�
	{
		std::wstring fullPath = m_modelPath + filename + L".mesh";
		std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
		file->Open(fullPath, FileMode::Read);

		// bone
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr<Ideal::IdealBone> bone = std::make_shared<Ideal::IdealBone>();
				bone->SetBoneIndex(file->Read<int32>());
				bone->SetName(file->Read<std::string>());
				bone->SetParent(file->Read<int32>());
				bone->SetTransform(file->Read<Matrix>());
				staticMesh->AddBone(bone);
			}
		}

		// SubMesh
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr <Ideal::IdealMesh<BasicVertex>> mesh = std::make_shared<Ideal::IdealMesh<BasicVertex>>();
				mesh->SetLocalTM(file->Read<Matrix>());

				mesh->SetName(file->Read<std::string>());
				mesh->SetBoneIndex(file->Read<int32>());

				// Material
				mesh->SetMaterialName(file->Read<std::string>());

				// vertex data
				{
					const uint32 count = file->Read<uint32>();
					std::vector<BasicVertex> vertices;
					vertices.resize(count);

					void* data = vertices.data();
					file->Read(&data, sizeof(BasicVertex) * count);
					mesh->AddVertices(vertices);
				}

				// index Data
				{
					const uint32 count = file->Read<uint32>();
					std::vector<uint32> indices;
					indices.resize(count);

					void* data = indices.data();
					file->Read(&data, sizeof(uint32) * count);
					mesh->AddIndices(indices);
				}

				// Bounds
				{
					Vector3 minBound = file->Read<Vector3>();
					Vector3 maxBound = file->Read<Vector3>();
					mesh->SetMinMaxBound(minBound, maxBound);
				}
				mesh->SetMaterial(m_defaultMaterial);
				staticMesh->AddMesh(mesh);
			}
		}
	}

	// Material
// 	{
// 		std::wstring fullPath = m_texturePath + filename + L".xml";
// 
// 		std::filesystem::path parentPath = std::filesystem::path(fullPath).parent_path();
// 
// 		//tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
// 		std::shared_ptr<tinyxml2::XMLDocument> document = std::make_shared<tinyxml2::XMLDocument>();
// 		tinyxml2::XMLError error = document->LoadFile(StringUtils::ConvertWStringToString(fullPath).c_str());
// 		assert(error == tinyxml2::XML_SUCCESS);
// 		if (error != tinyxml2::XML_SUCCESS)
// 		{
// 			MessageBox(NULL, fullPath.c_str(), L"read material", MB_OK);
// 		}
// 
// 		tinyxml2::XMLElement* root = document->FirstChildElement();
// 		tinyxml2::XMLElement* materialNode = root->FirstChildElement();
// 
// 		while (materialNode)
// 		{
// 			std::shared_ptr<Ideal::IdealMaterial> material = std::make_shared<Ideal::IdealMaterial>();
// 
// 			tinyxml2::XMLElement* node = nullptr;
// 
// 			node = materialNode->FirstChildElement();
// 			material->SetName((node->GetText()));
// 
// 			// DiffuseTexture
// 			node = node->NextSiblingElement();
// 			if (node->GetText())
// 			{
// 				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
// 				if (textureStr.length() > 0)
// 				{
// 					// Temp 2024.04.20
// 					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
// 
// 					material->SetDiffuseTextureFile(finalTextureStr);
// 				}
// 			}
// 
// 			// Specular Texture
// 			node = node->NextSiblingElement();
// 			if (node->GetText())
// 			{
// 				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
// 				if (textureStr.length() > 0)
// 				{
// 					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
// 					material->SetSpecularTextureFile(finalTextureStr);
// 					// TODO : Texture�����
// 					//auto texture
// 				}
// 			}
// 
// 			// Normal Texture
// 			node = node->NextSiblingElement();
// 			if (node->GetText())
// 			{
// 				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
// 				if (textureStr.length() > 0)
// 				{
// 					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
// 					material->SetNormalTextureFile(finalTextureStr);
// 					// TODO : Texture�����
// 					//auto texture
// 				}
// 			}
// 
// 			// Metalic Texture
// 			node = node->NextSiblingElement();
// 			if (node->GetText())
// 			{
// 				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
// 				if (textureStr.length() > 0)
// 				{
// 					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
// 					material->SetMetallicTextureFile(finalTextureStr);
// 				}
// 			}
// 
// 			// Roughness Texture
// 			node = node->NextSiblingElement();
// 			if (node->GetText())
// 			{
// 				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
// 				if (textureStr.length() > 0)
// 				{
// 					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
// 					material->SetRoughnessTextureFile(finalTextureStr);
// 				}
// 			}
// 
// 			// Ambient
// 			{
// 				node = node->NextSiblingElement();
// 
// 				Color color;
// 				color.x = node->FloatAttribute("R");
// 				color.y = node->FloatAttribute("G");
// 				color.z = node->FloatAttribute("B");
// 				color.w = node->FloatAttribute("A");
// 				material->SetAmbient(color);
// 			}
// 
// 			// Diffuse
// 			{
// 				node = node->NextSiblingElement();
// 
// 				Color color;
// 				color.x = node->FloatAttribute("R");
// 				color.y = node->FloatAttribute("G");
// 				color.z = node->FloatAttribute("B");
// 				color.w = node->FloatAttribute("A");
// 				material->SetDiffuse(color);
// 			}
// 
// 			// Specular
// 			{
// 				node = node->NextSiblingElement();
// 
// 				Color color;
// 				color.x = node->FloatAttribute("R");
// 				color.y = node->FloatAttribute("G");
// 				color.z = node->FloatAttribute("B");
// 				color.w = node->FloatAttribute("A");
// 				material->SetSpecular(color);
// 			}
// 
// 			// Emissive
// 			{
// 				node = node->NextSiblingElement();
// 
// 				Color color;
// 				color.x = node->FloatAttribute("R");
// 				color.y = node->FloatAttribute("G");
// 				color.z = node->FloatAttribute("B");
// 				color.w = node->FloatAttribute("A");
// 				material->SetEmissive(color);
// 			}
// 
// 			// Metallic
// 			{
// 				node = node->NextSiblingElement();
// 				float MetallicFactor = node->FloatAttribute("Factor");
// 				material->SetMetallicFactor(MetallicFactor);
// 			}
// 
// 			// Roughness
// 			{
// 				node = node->NextSiblingElement();
// 				float RoughnessFactor = node->FloatAttribute("Factor");
// 				material->SetRoughnessFactor(RoughnessFactor);
// 			}
// 
// 			// UseTextureInfo
// 			{
// 				node = node->NextSiblingElement();
// 				bool diffuse = node->BoolAttribute("Diffuse");
// 				bool normal = node->BoolAttribute("Normal");
// 				bool metallic = node->BoolAttribute("Metallic");
// 				bool roughness = node->BoolAttribute("Roughness");
// 				material->SetIsUseDiffuse(diffuse);
// 				material->SetIsUseNormal(normal);
// 				material->SetIsUseMetallic(metallic);
// 				material->SetIsUseRoughness(roughness);
// 				//material->SetIsUseDiffuse(true);
// 				//material->SetIsUseNormal(true);
// 				//material->SetIsUseMetallic(true);
// 				//material->SetIsUseRoughness(true);
// 			}
// 
// 			{
// 				// Material Id Allocate
// 				//material->SetID(AllocateMaterialID());
// 				//
// 				//material->SetBaseMap(m_defaultAlbedo);
// 				//material->SetNormalMap(m_defaultNormal);
// 				//material->SetMaskMap(m_defaultMask);
// 
// 				//staticMesh->AddMaterial(m_defaultMaterial);
// 			}
// 			materialNode = materialNode->NextSiblingElement();
// 		}
// 	}

	// Binding info
	staticMesh->FinalCreate(shared_from_this());
	OutMesh->SetStaticMesh(staticMesh);

	m_staticMeshes[key] = staticMesh;
}

void Ideal::ResourceManager::CreateSkinnedMeshObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> OutMesh, const std::wstring& filename)
{
	// �̹� ���� ���
	std::string key = StringUtils::ConvertWStringToString(filename);
	std::shared_ptr<Ideal::IdealSkinnedMesh> skinnedMesh = m_skinnedMeshes[key];

	if (skinnedMesh != nullptr)
	{
		skinnedMesh->AddRefCount();
		OutMesh->SetSkinnedMesh(skinnedMesh);
		return;
	}
	skinnedMesh = std::make_shared<Ideal::IdealSkinnedMesh>();
	skinnedMesh->AddRefCount();
	// ������ StaticMesh�� ���� ������ �־��ָ�ȴ�
	{
		std::wstring fullPath = m_modelPath + filename + L".dmesh";
		std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
		file->Open(fullPath, FileMode::Read);

		// bone
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr<Ideal::IdealBone> bone = std::make_shared<Ideal::IdealBone>();
				bone->SetBoneIndex(file->Read<int32>());
				bone->SetName(file->Read<std::string>());
				bone->SetParent(file->Read<int32>());
				bone->SetTransform(file->Read<Matrix>());
				bone->SetOffsetMatrix(file->Read<Matrix>());
				skinnedMesh->AddBone(bone);
			}
		}

		// SubMesh
		{
			const uint32 count = file->Read<uint32>();

			for (uint32 i = 0; i < count; ++i)
			{
				std::shared_ptr <Ideal::IdealMesh<SkinnedVertex>> mesh = std::make_shared<Ideal::IdealMesh<SkinnedVertex>>();
				// mesh->SetLocalTM(file->Read<Matrix>());

				mesh->SetName(file->Read<std::string>());
				mesh->SetBoneIndex(file->Read<int32>());

				// Material
				mesh->SetMaterialName(file->Read<std::string>());

				// vertex data
				{
					const uint32 count = file->Read<uint32>();
					std::vector<SkinnedVertex> vertices;
					vertices.resize(count);

					void* data = vertices.data();
					file->Read(&data, sizeof(SkinnedVertex) * count);
					mesh->AddVertices(vertices);
				}

				// index Data
				{
					const uint32 count = file->Read<uint32>();
					std::vector<uint32> indices;
					indices.resize(count);

					void* data = indices.data();
					file->Read(&data, sizeof(uint32) * count);
					mesh->AddIndices(indices);
				}
				mesh->SetMaterial(m_defaultMaterial);
				skinnedMesh->AddMesh(mesh);
			}
		}
	}

	// Material
	{
		std::wstring fullPath = m_texturePath + filename + L".xml";

		std::filesystem::path parentPath = std::filesystem::path(fullPath).parent_path();

		//tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
		std::shared_ptr<tinyxml2::XMLDocument> document = std::make_shared<tinyxml2::XMLDocument>();
		tinyxml2::XMLError error = document->LoadFile(StringUtils::ConvertWStringToString(fullPath).c_str());
		assert(error == tinyxml2::XML_SUCCESS);
		if (error != tinyxml2::XML_SUCCESS)
		{
			MessageBox(NULL, fullPath.c_str(), L"read material", MB_OK);
		}

		tinyxml2::XMLElement* root = document->FirstChildElement();
		tinyxml2::XMLElement* materialNode = root->FirstChildElement();

		while (materialNode)
		{
			std::shared_ptr<Ideal::IdealMaterial> material = std::make_shared<Ideal::IdealMaterial>();

			tinyxml2::XMLElement* node = nullptr;

			node = materialNode->FirstChildElement();
			material->SetName((node->GetText()));

			// DiffuseTexture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					// Temp 2024.04.20
					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;

					material->SetDiffuseTextureFile(finalTextureStr);
				}
			}

			// Specular Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
					material->SetSpecularTextureFile(finalTextureStr);
					// TODO : Texture�����
					//auto texture
				}
			}

			// Normal Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
					material->SetNormalTextureFile(finalTextureStr);
					// TODO : Texture�����
					//auto texture
				}
			}
			// Metalic Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
					material->SetMetallicTextureFile(finalTextureStr);
				}
			}

			// Roughness Texture
			node = node->NextSiblingElement();
			if (node->GetText())
			{
				std::wstring textureStr = StringUtils::ConvertStringToWString(node->GetText());
				if (textureStr.length() > 0)
				{
					std::wstring finalTextureStr = parentPath.wstring() + L"/" + textureStr;
					material->SetRoughnessTextureFile(finalTextureStr);
				}
			}

			// Ambient
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->SetAmbient(color);
			}

			// Diffuse
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->SetDiffuse(color);
			}

			// Specular
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->SetSpecular(color);
			}

			// Emissive
			{
				node = node->NextSiblingElement();

				Color color;
				color.x = node->FloatAttribute("R");
				color.y = node->FloatAttribute("G");
				color.z = node->FloatAttribute("B");
				color.w = node->FloatAttribute("A");
				material->SetEmissive(color);
			}

			// Metallic
			{
				node = node->NextSiblingElement();
				float MetallicFactor = node->FloatAttribute("Factor");
				material->SetMetallicFactor(MetallicFactor);
			}

			// Roughness
			{
				node = node->NextSiblingElement();
				float RoughnessFactor = node->FloatAttribute("Factor");
				material->SetRoughnessFactor(RoughnessFactor);
			}

			// UseTextureInfo
			{
				node = node->NextSiblingElement();
				bool diffuse = node->BoolAttribute("Diffuse");
				bool normal = node->BoolAttribute("Normal");
				bool metallic = node->BoolAttribute("Metallic");
				bool roughness = node->BoolAttribute("Roughness");
				//material->SetIsUseDiffuse(diffuse);
				//material->SetIsUseNormal(normal);
				//material->SetIsUseMetallic(metallic);
				//material->SetIsUseRoughness(roughness);
				material->SetIsUseDiffuse(true);
				material->SetIsUseNormal(true);
				material->SetIsUseMetallic(true);
				material->SetIsUseRoughness(true);
			}

			{
				// Material Id Allocate
				//material->SetID(AllocateMaterialID());
				//
				//material->SetBaseMap(m_defaultAlbedo);
				//material->SetNormalMap(m_defaultNormal);
				//material->SetMaskMap(m_defaultMask);

				//skinnedMesh->AddMaterial(m_defaultMaterial);
			}
			materialNode = materialNode->NextSiblingElement();
		}
	}

	// Binding info
	skinnedMesh->FinalCreate(shared_from_this());

	OutMesh->SetSkinnedMesh(skinnedMesh);

	// KeyBinding
	m_skinnedMeshes[key] = skinnedMesh;
}

std::shared_ptr<Ideal::D3D12Shader> ResourceManager::CreateAndLoadShader(const std::wstring& FilePath)
{
	std::shared_ptr<Ideal::ShaderManager> shaderManager = std::make_shared<Ideal::ShaderManager>();
	std::shared_ptr<Ideal::D3D12Shader> shader = std::make_shared<Ideal::D3D12Shader>();
	shaderManager->Init();
	shaderManager->LoadShaderFile(FilePath, shader);
	return shader;
}

std::shared_ptr<Ideal::D3D12VertexBuffer> ResourceManager::GetDebugLineVB()
{
	return m_debugLineVertexBuffer;
}

void ResourceManager::CreateDefaultDebugLine()
{
	GeometryVertex g1; g1.Position = Vector3(0, 0, 0);
	GeometryVertex g2; g2.Position = Vector3(1, 0, 0);
	std::vector<GeometryVertex> vertices = { g1, g2 };

	m_debugLineVertexBuffer = std::make_shared<Ideal::D3D12VertexBuffer>();

	CreateVertexBuffer<GeometryVertex>(m_debugLineVertexBuffer, vertices);
}

std::shared_ptr<Ideal::IMesh> ResourceManager::CreateParticleMesh(const std::wstring& filename)
{
	std::shared_ptr<IdealMesh<ParticleMeshVertex>> mesh = std::make_shared<IdealMesh<ParticleMeshVertex>>();

	std::wstring fullPath = m_modelPath + filename + L".pmesh";
	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	mesh->SetName(file->Read<std::string>());

	// vertex data
	{
		const uint32 count = file->Read<uint32>();
		std::vector<ParticleMeshVertex> vertices;
		vertices.resize(count);

		void* data = vertices.data();
		file->Read(&data, sizeof(ParticleMeshVertex) * count);
		mesh->AddVertices(vertices);
	}

	// index Data
	{
		const uint32 count = file->Read<uint32>();
		std::vector<uint32> indices;
		indices.resize(count);

		void* data = indices.data();
		file->Read(&data, sizeof(uint32) * count);
		mesh->AddIndices(indices);
	}
	mesh->Create(shared_from_this());
	return std::static_pointer_cast<Ideal::IMesh>(mesh);
}

void ResourceManager::DeleteStaticMeshObject(std::shared_ptr<Ideal::IdealStaticMeshObject> Mesh)
{
	std::wstring name = Mesh->GetName();
	std::string key = StringUtils::ConvertWStringToString(name);
	m_staticMeshes[key]->SubRefCount();

	if (m_staticMeshes[key]->GetRefCount() == 0)
	{
		// todo : delete
		auto it = m_staticMeshes.find(key);
		if (it != m_staticMeshes.end())
		{
			m_staticMeshes.erase(it);
		}
	}
}

void ResourceManager::DeleteSkinnedMeshObject(std::shared_ptr<Ideal::IdealSkinnedMeshObject> Mesh)
{
	std::wstring name = Mesh->GetName();
	std::string key = StringUtils::ConvertWStringToString(name);
	if (m_skinnedMeshes[key])
	{
		m_skinnedMeshes[key]->SubRefCount();
		if (m_skinnedMeshes[key]->GetRefCount() == 0)
		{
			// todo : delete
			auto it = m_skinnedMeshes.find(key);
			if (it != m_skinnedMeshes.end())
			{
				m_skinnedMeshes.erase(it);
			}
		}
	}
}

std::shared_ptr<Ideal::D3D12Texture> ResourceManager::GetDefaultMaskTexture()
{
	return m_defaultMask;
}

std::shared_ptr<Ideal::D3D12Texture> ResourceManager::GetDefaultNormalTexture()
{
	return m_defaultNormal;
}

std::shared_ptr<Ideal::D3D12Texture> ResourceManager::GetDefaultAlbedoTexture()
{
	return m_defaultAlbedo;
}

std::shared_ptr<Ideal::IdealMaterial> ResourceManager::GetDefaultMaterial()
{
	return m_defaultMaterial;
}

void ResourceManager::CreateDefaultMaterial()
{
	m_defaultMaterial = std::make_shared<Ideal::IdealMaterial>();
	m_defaultMaterial->SetBaseMap(m_defaultAlbedo);
	m_defaultMaterial->SetNormalMap(m_defaultNormal);
	m_defaultMaterial->SetMaskMap(m_defaultMask);
	m_defaultMaterial->SetID(AllocateMaterialID());
}

void ResourceManager::DeleteTexture(std::shared_ptr<Ideal::D3D12Texture> Texture)
{
	//m_textures[Texture->GetName()] = nullptr;
	m_textures.erase(Texture->GetName());
}

void ResourceManager::CreateDefaultTextures()
{
	CreateTexture(m_defaultAlbedo, L"../Resources/DefaultData/DefaultAlbedo.png");
	//CreateTexture(m_defaultNormal, L"../Resources/DefaultData/DefaultNormalMap.png", true, DXGI_FORMAT_R8G8B8A8_UNORM);
	CreateTexture(m_defaultNormal, L"../Resources/DefaultData/DefaultNormalMap.png", true);
	CreateTexture(m_defaultMask, L"../Resources/DefaultData/DefaultBlack.png");
}

std::shared_ptr<Ideal::IdealMaterial> ResourceManager::CreateMaterial()
{
	auto newMaterial = std::make_shared<Ideal::IdealMaterial>();
	newMaterial->SetID(AllocateMaterialID());
	return newMaterial;
}

void ResourceManager::CreateAnimation(std::shared_ptr<Ideal::IdealAnimation>& OutAnimation, const std::wstring& filename, const Matrix& offset/* = Matrix::Identity*/)
{
	std::string key = StringUtils::ConvertWStringToString(filename);
	std::shared_ptr<Ideal::IdealAnimation> animation = m_animations[key];

	if (animation != nullptr)
	{
		OutAnimation = animation;
		return;
	}

	std::wstring fullPath = m_modelPath + filename + L".anim";

	std::shared_ptr<FileUtils> file = std::make_shared<FileUtils>();
	file->Open(fullPath, FileMode::Read);

	OutAnimation->name = file->Read<std::string>();
	OutAnimation->duration = file->Read<float>();
	OutAnimation->frameRate = file->Read<float>();
	OutAnimation->frameCount = file->Read<uint32>();

	uint32 keyframeCount = file->Read<uint32>();

	for (uint32 i = 0; i < keyframeCount; ++i)
	{
		std::shared_ptr<ModelKeyframe> keyFrame = std::make_shared<ModelKeyframe>();
		keyFrame->boneName = file->Read<std::string>();

		uint32 size = file->Read<uint32>();

		if (size > 0)
		{
			keyFrame->transforms.resize(size);
			void* ptr = &keyFrame->transforms[0];
			file->Read(&ptr, sizeof(Ideal::ModelKeyFrameData) * size);
		}

		OutAnimation->keyframes[keyFrame->boneName] = keyFrame;
	}

	// Create Bone Transform
	int32 numBones = OutAnimation->numBones = file->Read<int32>();
	std::vector<std::shared_ptr<Ideal::IdealBone>> bones;
	bones.resize(numBones);

	// bone
	{
		for (uint32 i = 0; i < (uint32)numBones; ++i)
		{
			std::shared_ptr<Ideal::IdealBone> bone = std::make_shared<Ideal::IdealBone>();
			bone->SetBoneIndex(file->Read<int32>());
			bone->SetName(file->Read<std::string>());
			bone->SetParent(file->Read<int32>());
			bone->SetTransform(file->Read<Matrix>());
			bones[i] = bone;
		}
	}

	// BoneTransform
	{
		std::vector<Matrix> tempAnimBoneTransforms(MAX_BONE_TRANSFORMS, Matrix::Identity);
		std::shared_ptr<Ideal::AnimTransform> animTransform = std::make_shared<Ideal::AnimTransform>();

		for (uint32 frame = 0; frame < OutAnimation->frameCount; ++frame)
		{
			for (uint32 boneIdx = 0; boneIdx < (uint32)OutAnimation->numBones; ++boneIdx)
			{
				std::shared_ptr<Ideal::IdealBone> bone = bones[boneIdx];

				Matrix matAnimation;
				//Matrix matAnimation = bone->GetTransform();

				std::shared_ptr<ModelKeyframe> keyFrame = OutAnimation->GetKeyframe(bone->GetName());

				if (keyFrame != nullptr)
				{
					ModelKeyFrameData& data = keyFrame->transforms[frame];

					Matrix S, R, T;
					S = Matrix::CreateScale(data.scale.x, data.scale.y, data.scale.z);
					R = Matrix::CreateFromQuaternion(data.rotation);
					T = Matrix::CreateTranslation(data.translation.x, data.translation.y, data.translation.z);

					matAnimation = S * R * T;
				}
				else
				{
					matAnimation = Matrix::Identity;
					matAnimation = bone->GetTransform();
				}

				// ������
				Matrix toRootMatrix = bone->GetTransform();
				Matrix invGlobal = toRootMatrix.Invert();

				int32 parentIndex = bone->GetParent();

				Matrix matParent = Matrix::Identity;
				if (parentIndex >= 0)
				{
					matParent = tempAnimBoneTransforms[parentIndex];
				}
				else
				{
					matParent = offset;
				}

				tempAnimBoneTransforms[boneIdx] = matAnimation * matParent;

				animTransform->transforms[frame][boneIdx] = tempAnimBoneTransforms[boneIdx];
			}
		}
		OutAnimation->m_animTransform = animTransform;
	}

	// Final Insert Key
	m_animations[key] = OutAnimation;
}

void ResourceManager::CreateDefaultQuadMesh()
{
	std::vector<SimpleVertex> vertices =
	{
		{ { 0.0f, 1.0f, 0.0f, 1.f }, { 0.0f, 1.0f } },
		{ { 0.0f, 0.0f, 0.0f, 1.f }, { 0.0f, 0.0f } },
		{ { 1.0f, 0.0f, 0.0f, 1.f }, { 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 0.0f, 1.f }, { 1.0f, 1.0f } },
	};

	std::vector<uint32> Indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	m_defaultQuadMesh = std::make_shared<Ideal::IdealMesh<SimpleVertex>>();
	m_defaultQuadMesh->AddVertices(vertices);
	m_defaultQuadMesh->AddIndices(Indices);
	m_defaultQuadMesh->Create(shared_from_this());
}

std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> ResourceManager::GetDefaultQuadMesh()
{
	return m_defaultQuadMesh;
}

void ResourceManager::CreateDefaultQuadMesh2()
{
	std::vector<SimpleVertex> vertices =
	{
		{ { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },    // Bottom left -> Top left.
		{ { -1.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },    // Top left -> Bottom left.
		{ { 1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },    // Bottom right -> Top right.
		{ { 1.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }      // Top right -> Bottom right.
	};

	std::vector<uint32> Indices =
	{
		0, 1, 2,
		1, 3, 2
	};

	m_defaultQuadMesh2 = std::make_shared<Ideal::IdealMesh<SimpleVertex>>();
	m_defaultQuadMesh2->AddVertices(vertices);
	m_defaultQuadMesh2->AddIndices(Indices);
	m_defaultQuadMesh2->Create(shared_from_this());
}

std::shared_ptr<Ideal::IdealMesh<SimpleVertex>> ResourceManager::GetDefaultQuadMesh2()
{
	return m_defaultQuadMesh2;
}

void ResourceManager::CreateParticleVertexBuffer()
{
	std::vector<ParticleVertex> vertices;
	vertices.resize(ParticleCount);
	for (uint32 i = 0; i < ParticleCount; ++i)
	{
		vertices[i].Color = Vector4(1.f, 1.f, 0.2f, 1.f);
	}
	m_particleVertexBuffer = std::make_shared<Ideal::D3D12VertexBuffer>();
	CreateVertexBuffer<ParticleVertex>(m_particleVertexBuffer, vertices);
}

void ResourceManager::CreateParticleBuffers()
{
	// ������ī��Ʈ��ŭ ���� �ʿ� ����. ������ �����ؼ� ���� ������..
	// ���ε� ���۸� �����
	// ��ġ ���۸� �о� �ְ�
	// GPU�� ���ε��ϴ� ���۸� �����.
}

std::shared_ptr<Ideal::D3D12VertexBuffer> ResourceManager::GetParticleVertexBuffer()
{
	return m_particleVertexBuffer;
}

