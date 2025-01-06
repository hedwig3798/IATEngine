#pragma once
#include "Component.h"

namespace Ideal
{
	class IMeshObject;
	class IMesh;
	class IMaterial;
}

namespace Truth
{
	struct Material;
}

/// <summary>
/// Static Mehs Class
/// </summary>
namespace Truth
{
	class Mesh :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Mesh);

	private:
		std::shared_ptr<Ideal::IMeshObject> m_mesh;
		std::vector<std::shared_ptr<Ideal::IMesh>> m_subMesh;

		PROPERTY(mat);
		std::vector<std::shared_ptr<Material>> m_mat;
		std::vector<std::string> m_matPath;

		PROPERTY(path);
		std::wstring m_path;

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(isStatic);
		bool m_isStatic;

	public:
		Mesh();
		Mesh(std::wstring _path);
		virtual ~Mesh();

		void SetMesh(std::wstring _path);
		void SetMesh();
		void SetRenderable(bool _isRenderable);

		METHOD(Initialize);
		void Initialize();

		METHOD(ApplyTransform);
		void ApplyTransform();

		void SetMeshTransformMatrix();
		void SetMeshTransformMatrix(const Matrix& _m);

		METHOD(Update);
		void Update();

		METHOD(SetActive);
		void SetActive();

		METHOD(Destroy);
		void Destroy();

		void DeleteMesh();

		void SetMaterialByIndex(uint32 _index, std::string _material);
		std::vector<uint64> GetMaterialImagesIDByIndex(uint32 _index);

		Matrix GetMeshLocalTM();

		void SetStatic(bool _isStatic);

#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE
	};
}
