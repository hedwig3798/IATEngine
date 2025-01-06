#include "DXRAccelerationStructure.h"
#include "GraphicsEngine/D3D12/D3D12Resource.h"
#include "GraphicsEngine/D3D12/DeferredDeleteManager.h"
#include "GraphicsEngine/Resource/IdealMaterial.h"

inline void AllocateUAVBuffer(ID3D12Device* pDevice, UINT64 bufferSize, ID3D12Resource** ppResource, D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, const wchar_t* resourceName = nullptr)
{
	auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
	Check(pDevice->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		initialResourceState,
		nullptr,
		IID_PPV_ARGS(ppResource)));
	if (resourceName)
	{
		(*ppResource)->SetName(resourceName);
	}
}

Ideal::DXRBottomLevelAccelerationStructure::DXRBottomLevelAccelerationStructure(const std::wstring& Name)
{
	m_name = Name;
}

Ideal::DXRBottomLevelAccelerationStructure::~DXRBottomLevelAccelerationStructure()
{
	
}

void Ideal::DXRBottomLevelAccelerationStructure::Create(ComPtr<ID3D12Device5> Device, std::vector<BLASGeometry>& Geometries, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, bool AllowUpdate)
{
	m_allowUpdate = AllowUpdate;

	m_buildFlags = BuildFlags;
	if (AllowUpdate)
		m_buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;

	// build geometry desc
	BuildGeometries(Geometries);

	// Prebuild
	{
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = {};
		bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		bottomLevelInputs.Flags = m_buildFlags;
		bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		bottomLevelInputs.NumDescs = static_cast<uint32>(m_geometryDescs.size());
		bottomLevelInputs.pGeometryDescs = m_geometryDescs.data();

		Device->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &m_preBuildInfo);
		if (m_preBuildInfo.ResultDataMaxSizeInBytes <= 0) __debugbreak();
	}

	m_accelerationStructure = std::make_shared<Ideal::D3D12UAVBuffer>();
	m_accelerationStructure->Create(
		Device.Get(),
		m_preBuildInfo.ResultDataMaxSizeInBytes,
		D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
		m_name.c_str()
	);
	m_isDirty = true;
	m_isBuilt = false;
}

void Ideal::DXRBottomLevelAccelerationStructure::FreeMyHandle()
{
	// Fixed Descriptor ���� ������ �� ���ҽ����� ��ȯ�Ѵ�.
	for (auto geometry : m_geometries)
	{
		geometry.SRV_IndexBuffer.Free();
		geometry.SRV_VertexBuffer.Free();
		//geometry.SRV_Diffuse.Free();
		//geometry.SRV_Normal.Free();
		//geometry.SRV_Metallic.Free();
		//geometry.SRV_Roughness.Free();
		//geometry.SRV_Mask.Free();
		//geometry.CBV_MaterialInfo.Free();
		//geometry.Material->Free();
	}

	m_geometries.clear();
}

void Ideal::DXRBottomLevelAccelerationStructure::Build(ComPtr<ID3D12GraphicsCommandList4> CommandList, ComPtr<ID3D12Resource> ScratchBuffer)
{
	// scratch ����
	// 
	uint64 scratchBufferSizeInBytes = ScratchBuffer->GetDesc().Width;
	if (m_preBuildInfo.ScratchDataSizeInBytes > scratchBufferSizeInBytes)
	{
		std::wstring msg = L"Failed to Build BLAS : ";
		msg += m_name;
		MyMessageBoxW(msg.c_str());
		__debugbreak();
	}

	m_currentID = (m_currentID + 1) % MAX_PENDING_COUNT;
	m_cacheGeometryDescs[m_currentID].clear();
	m_cacheGeometryDescs[m_currentID].resize(m_geometryDescs.size());
	copy(m_geometryDescs.begin(), m_geometryDescs.end(), m_cacheGeometryDescs[m_currentID].begin());

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC blasBuildDesc = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& blasInputs = blasBuildDesc.Inputs;

	blasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	blasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	blasInputs.Flags = m_buildFlags;

	// �̹� ����Ǿ��� ��츸 �Ʒ��� �÷��� ���� ����
	if (m_isBuilt && m_allowUpdate)
	{
		blasInputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
		// ����尡 �ƴ� ������Ʈ �� ��� �Ʒ��� source�� �߰��ؾ���
		blasBuildDesc.SourceAccelerationStructureData = m_accelerationStructure->GetGPUVirtualAddress();
	}
	blasInputs.NumDescs = static_cast<uint32>(m_cacheGeometryDescs[m_currentID].size());
	blasInputs.pGeometryDescs = m_cacheGeometryDescs[m_currentID].data();

	blasBuildDesc.ScratchAccelerationStructureData = ScratchBuffer->GetGPUVirtualAddress();
	blasBuildDesc.DestAccelerationStructureData = m_accelerationStructure->GetGPUVirtualAddress();

	CommandList->BuildRaytracingAccelerationStructure(&blasBuildDesc, 0, nullptr);

	m_isDirty = false;	// �ٽ� �����ϱ� ������ ���
	m_isBuilt = true;	// �̹� ��������ٰ� ����
}

void Ideal::DXRBottomLevelAccelerationStructure::BuildGeometries(std::vector<BLASGeometry>& Geometries)
{
	D3D12_RAYTRACING_GEOMETRY_DESC geometryDescTemplate = {};
	geometryDescTemplate.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
	geometryDescTemplate.Triangles.IndexFormat = INDEX_FORMAT;
	geometryDescTemplate.Triangles.VertexFormat = VERTEX_FORMAT;
	m_geometryDescs.reserve(Geometries.size());

	m_geometryDescs.clear();
	for (BLASGeometry& geometry : Geometries)
	{
		D3D12_RAYTRACING_GEOMETRY_DESC& geometryDesc = geometryDescTemplate;


		// 2024.10.16 ����. �ϰ����� ���� �ȵȴ�. ���� ��� ���� BLAS�� ���� �ִµ� �������Ʈ�� ó���� �����ϰ� alphaclippingüũ�� �����ϸ�
		// ���� �ν��Ͻ����� ������ ������ �ȴ�. alpha clipping üũ�� ���ص�. ���� �׳� ó���� ������ ����ũ�� ����� alpha clipping üũ�� 
		// ������ �����ϰ� �Ѵ�.
		// ���⼭ ���� ������Ʈ���� �˻��Ͽ� Flag�� �����Ѵ�.
		//bool IsAlphaClipping = geometry.Material.lock()->GetIsAlphaClipping();
		//if (IsAlphaClipping)
		//{
		//	// Any Hit Shader ����
		//	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE;
		//}
		//else
		//{
		//	// Any Hit Shader ����
		//	geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
		//}
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
		geometryDesc.Triangles.IndexBuffer = geometry.IndexBufferGPUAddress;
		geometryDesc.Triangles.IndexCount = geometry.IndexCount;
		geometryDesc.Triangles.VertexBuffer.StartAddress = geometry.VertexBufferGPUAddress;
		geometryDesc.Triangles.VertexBuffer.StrideInBytes = geometry.VertexStrideInBytes;
		geometryDesc.Triangles.VertexCount = geometry.VertexCount;

		// �������� ���� desc�� ����
		m_geometryDescs.push_back(geometryDesc);
	}

	m_geometries = Geometries;
}

//------------------------TLAS------------------------//

Ideal::DXRTopLevelAccelerationStructure::DXRTopLevelAccelerationStructure(const std::wstring& Name)
{
	m_name = Name;
}

Ideal::DXRTopLevelAccelerationStructure::~DXRTopLevelAccelerationStructure()
{

}

void Ideal::DXRTopLevelAccelerationStructure::Create(ComPtr<ID3D12Device5> Device, uint32 NumBLASInstanceDescs, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS BuildFlags, std::shared_ptr<Ideal::DeferredDeleteManager> DeferredDeleteManager, bool AllowUpdate /*= false */)
{
	m_allowUpdate = AllowUpdate;
	m_buildFlags = BuildFlags;
	if (AllowUpdate)
	{
		m_buildFlags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
	}

	// prebuild
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.Flags = m_buildFlags;
	topLevelInputs.NumDescs = NumBLASInstanceDescs;

	Device->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &m_preBuildInfo);
	if (m_preBuildInfo.ResultDataMaxSizeInBytes <= 0)
	{
		MyMessageBoxW(L"Failed to prebuild AS");
	}

	D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
	//D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON;

	if (m_accelerationStructure == nullptr)
	{
		m_accelerationStructure = std::make_shared<Ideal::D3D12UAVBuffer>();
	}
	/*if (DeferredDeleteManager)
	{
		DeferredDeleteManager->AddD3D12ResourceToDelete(m_accelerationStructure->GetResource());
		m_accelerationStructure = std::make_shared<Ideal::D3D12UAVBuffer>();
	}*/
	m_accelerationStructure->Create(Device.Get(), m_preBuildInfo.ResultDataMaxSizeInBytes, initialResourceState, m_name.c_str());
}

void Ideal::DXRTopLevelAccelerationStructure::Build(ComPtr<ID3D12GraphicsCommandList4> CommandList, uint32 NumInstanceDesc, D3D12_GPU_VIRTUAL_ADDRESS InstanceDescsGPUAddress, ComPtr<ID3D12Resource> ScratchBuffer)
{

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC tlasBuildDesc = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& tlasInputs = tlasBuildDesc.Inputs;
	{
		tlasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		tlasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		tlasInputs.Flags = m_buildFlags;
		if (m_isBuilt && m_allowUpdate)
		{
			// 2024.07.08 Update Flag�� �� ������ ���ε� �� ������ ����带 �ϴ� �ȵ��´�.
			tlasInputs.Flags |= D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;

			// ���ÿ����� �� �ڵ尡 ������ �̰� �����ָ� ERROR ��
			tlasBuildDesc.SourceAccelerationStructureData = m_accelerationStructure->GetGPUVirtualAddress();
		}
		tlasInputs.NumDescs = NumInstanceDesc;
		tlasInputs.InstanceDescs = InstanceDescsGPUAddress;

		tlasBuildDesc.ScratchAccelerationStructureData = ScratchBuffer->GetGPUVirtualAddress();
		tlasBuildDesc.DestAccelerationStructureData = m_accelerationStructure->GetGPUVirtualAddress();
	}

	CommandList->BuildRaytracingAccelerationStructure(&tlasBuildDesc, 0, nullptr);
	m_isDirty = false;
	m_isBuilt = true;
}