#pragma once
#include "Headers.h"

namespace Truth
{
	class NavGeom
	{
	public:
		std::vector<float> m_ver;
		std::vector<int32> m_inx;

		std::vector<float> m_bmin;
		std::vector<float> m_bmax;


	public:
		NavGeom();
		~NavGeom();

		void Destroy();

		void Load(const std::wstring& _path);
		void Load(const std::vector<float>& _points, const std::vector<uint32>& _indices);
	};
}

