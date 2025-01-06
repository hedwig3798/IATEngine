#include "NavGeom.h"
#include "TFileUtils.h"

Truth::NavGeom::NavGeom()
	: m_bmin(std::vector<float>(3, FLT_MAX))
	, m_bmax(std::vector<float>(3, -FLT_MAX))
	, m_ver(std::vector<float>())
	, m_inx(std::vector<int32>())
{

}

Truth::NavGeom::~NavGeom()
{
	Destroy();
}

void Truth::NavGeom::Destroy()
{
	m_ver.clear();
	m_inx.clear();

	m_bmin.clear();
	m_bmax.clear();
}

void Truth::NavGeom::Load(const std::wstring& _path)
{
	std::shared_ptr<TFileUtils> file = std::make_shared<TFileUtils>();
	file->Open(_path, FileMode::Read);

	uint32 acc = 0;
	unsigned int meshNum = file->Read<unsigned int>();
	for (unsigned int i = 0; i < meshNum; i++)
	{
		unsigned int verticesNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < verticesNum; j++)
		{
			Vector3 p;
			p.x = file->Read<float>();
			p.y = file->Read<float>();
			p.z = file->Read<float>();

			m_ver.push_back(p.x);
			m_ver.push_back(p.y);
			m_ver.push_back(p.z);

			m_bmax[0] = max(m_bmax[0], p.x);
			m_bmax[1] = max(m_bmax[1], p.y);
			m_bmax[2] = max(m_bmax[2], p.z);

			m_bmin[0] = min(m_bmin[0], p.x);
			m_bmin[1] = min(m_bmin[1], p.y);
			m_bmin[2] = min(m_bmin[2], p.z);
		}

		unsigned int indNum = file->Read<unsigned int>();
		for (unsigned int j = 0; j < indNum; j++)
		{
			m_inx.push_back(file->Read<uint32>() + acc);
		}

		acc += verticesNum;
	}
}

void Truth::NavGeom::Load(const std::vector<float>& _points, const std::vector<uint32>& _indices)
{
	for (size_t j = 0; j < _points.size(); j += 3)
	{
		float x = _points[j];
		float y = _points[j + 1];
		float z = _points[j + 2];

		m_ver.push_back(x);
		m_ver.push_back(y);
		m_ver.push_back(z);

		m_bmax[0] = max(m_bmax[0], x);
		m_bmax[1] = max(m_bmax[1], y);
		m_bmax[2] = max(m_bmax[2], z);

		m_bmin[0] = min(m_bmin[0], x);
		m_bmin[1] = min(m_bmin[1], y);
		m_bmin[2] = min(m_bmin[2], z);
	}

	for (size_t j = 0; j < _indices.size(); j++)
	{
		m_inx.push_back(_indices[j]);
	}
}
