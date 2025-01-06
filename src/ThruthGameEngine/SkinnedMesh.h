#pragma once
#include "Component.h"

namespace Ideal
{
	class IBone;
}

namespace Truth
{
	class SkinnedMesh :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(SkinnedMesh);

	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	private:
		std::weak_ptr<Ideal::ISkinnedMeshObject> m_skinnedMesh;
		std::weak_ptr<Ideal::IAnimation> m_animation;

		std::map<std::string, std::weak_ptr<Ideal::IBone>> m_boneMap;

		PROPERTY(mat);
		std::vector<std::shared_ptr<Material>> m_mat;
		std::vector<std::string> m_matPath;

	private:
		PROPERTY(path);
		std::wstring m_path;

		PROPERTY(isRendering);
		bool m_isRendering;

		PROPERTY(currentFrame);
		int m_currentFrame;

		PROPERTY(isAnimationPlaying);
		bool m_isAnimationPlaying;

		PROPERTY(isAnimationEnd);
		bool m_isAnimationEnd;

		PROPERTY(isAnimationChanged);
		bool m_isAnimationChanged;

		PROPERTY(animationMaxFrame);
		int m_animationMaxFrame;

		int m_oldFrame;

	public:
		SkinnedMesh();
		SkinnedMesh(std::wstring _path);
		virtual ~SkinnedMesh();

		void SetSkinnedMesh(std::wstring _path);
		void AddAnimation(std::string _name, std::wstring _path, const Matrix& _offset = Matrix::Identity);
		void SetAnimation(const std::string& _name, bool WhenCurrentAnimationFinished);
		void SetAnimationSpeed(float Speed);
		void SetPlayStop(bool playStop);

		METHOD(Initialize);
		void Initialize();

		METHOD(Update);
		void Update();

		METHOD(ApplyTransform);
		void ApplyTransform();

		METHOD(Destroy);
		void Destroy();

		METHOD(SetActive);
		void SetActive();

		inline int GetCurrentFrame() const { return m_currentFrame; }
		inline bool GetIsAnimationEnd() const { return m_isAnimationEnd; }
		inline bool GetIsAnimationChanged() const { return m_isAnimationChanged; }

		void DeleteMesh();

		std::weak_ptr<Ideal::IBone> GetBone(const std::string& _name);
		const std::map<std::string, std::weak_ptr<Ideal::IBone>>& GetBoneMap() { return m_boneMap; }
#ifdef EDITOR_MODE
		virtual void EditorSetValue();
#endif // EDITOR_MODE

	};

	template<class Archive>
	void Truth::SkinnedMesh::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
		if (file_version >= 1)
		{
			size_t matSize;
			_ar& matSize;
			for (size_t i = 0; i < matSize ; i++)
			{
				std::string matPath;
				_ar& matPath;
				m_matPath.push_back(matPath);
			}
		}
	}

	template<class Archive>
	void Truth::SkinnedMesh::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);
		_ar& m_path;
		_ar& m_mat.size();
		for (auto& m : m_mat)
		{
			_ar& m->m_path;
		}
	}
}

BOOST_CLASS_EXPORT_KEY(Truth::SkinnedMesh)
BOOST_CLASS_VERSION(Truth::SkinnedMesh, 1)