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
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

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

	template<class Archive>
	void Truth::Mesh::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
		if (file_version >= 1)
		{
			size_t matSize;
			_ar& matSize;
			for (size_t i = 0; i < matSize; i++)
			{
				std::string matPath;
				_ar& matPath;
				m_matPath.push_back(matPath);
			}
		}
		if (file_version >= 2)
			_ar& m_isStatic;
	}

	template<class Archive>
	void Truth::Mesh::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
		_ar& m_mat.size();
		for (auto& m : m_mat)
		{
			_ar& m->m_path;
		}
		_ar& m_isStatic;
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::Mesh)
BOOST_CLASS_VERSION(Truth::Mesh, 2)