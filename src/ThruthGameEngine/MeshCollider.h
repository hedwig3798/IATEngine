#pragma once
#include "Collider.h"
namespace Truth
{
	class MeshCollider 
		: public Collider
	{
		GENERATE_CLASS_TYPE_INFO(MeshCollider);

	private:
		std::vector<std::vector<Vector3>> m_points;
		std::vector<std::vector<int>> m_index;
		std::wstring m_path;
		std::vector<physx::PxShape*> m_meshCollider;

		bool m_isConvex;

	public:
		MeshCollider();
		MeshCollider(std::string _path, bool _isConvex = false);
		~MeshCollider();

		METHOD(Initialize);
		void Initialize();

		METHOD(Awake);
		virtual void Awake() override;

	private:
		void GetPoints();

	};

}

