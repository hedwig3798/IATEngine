#include "Material.h"
#include "IMaterial.h"
#include "Texture.h"
#include "GraphicsManager.h"
#include "TFileUtils.h"
#include "managers.h"
#include <yaml-cpp/yaml.h>

void Truth::Material::SetTexture()
{
	if (m_baseMap)
		m_material->SetBaseMap(m_baseMap->m_texture);
	if (m_normalMap)
		m_material->SetNormalMap(m_normalMap->m_texture);
	if (m_maskMap)
		m_material->SetMaskMap(m_maskMap->m_texture);

	m_material->SetTiling(m_tileX, m_tileY);
	m_material->SetAlphaClipping(m_alphaCulling);
	m_material->SetSurfaceTypeTransparent(m_transparent);

	m_material->ChangeLayerBitMask(m_layer);
}

void Truth::Material::ChangeTexture(std::wstring _path, int _type)
{
	fs::path p = fs::path(_path).extension();
	if (!(
		p == ".png" ||
		p == ".PNG" ||
		p == ".tga" ||
		p == ".TGA")
		)
	{
		return;
	}
	auto m_tex = m_gp->CreateTexture(_path);
	switch (_type)
	{
	case 0:
		m_material->SetBaseMap(m_tex->m_texture);
		m_baseMap = m_tex;
		break;
	case 1:
		m_material->SetNormalMap(m_tex->m_texture);
		m_normalMap = m_tex;
		break;
	case 2:
		m_material->SetMaskMap(m_tex->m_texture);
		m_maskMap = m_tex;
		break;
	default:
		break;
	}
}

void Truth::Material::ChangeMaterial()
{
	memset(&m_openFileName, 0, sizeof(OPENFILENAME));
	m_openFileName.lStructSize = sizeof(OPENFILENAME);
	m_openFileName.hwndOwner = m_hwnd;
	m_openFileName.lpstrFile = m_fileBuffer;
	m_openFileName.nMaxFile = 256;
	m_openFileName.lpstrInitialDir = L".";

	if (GetOpenFileName(&m_openFileName) != 0)
	{
		fs::path matPath = m_openFileName.lpstrFile;
		if (matPath.extension() != ".matData")
		{
			::SetCurrentDirectory(Managers::GetRootPath().c_str());
			return;
		}
		auto mat = m_gp->CreateMaterial(matPath.generic_string());
		m_material = mat->m_material;
		m_baseMap = mat->m_baseMap;
		m_normalMap = mat->m_normalMap;
		m_maskMap = mat->m_maskMap;
		SetTexture();
	}
	::SetCurrentDirectory(Managers::GetRootPath().c_str());
	SaveMaterial();
}

void Truth::Material::ChangeTexture(int _type)
{
	memset(&m_openFileName, 0, sizeof(OPENFILENAME));
	m_openFileName.lStructSize = sizeof(OPENFILENAME);
	m_openFileName.hwndOwner = m_hwnd;
	m_openFileName.lpstrFile = m_fileBuffer;
	m_openFileName.nMaxFile = 256;
	m_openFileName.lpstrInitialDir = L".";

	if (GetOpenFileName(&m_openFileName) != 0)
	{
		ChangeTexture(m_openFileName.lpstrFile, _type);
	}
	::SetCurrentDirectory(Managers::GetRootPath().c_str());
	SaveMaterial();
}

void Truth::Material::SaveMaterial()
{
	std::shared_ptr<TFileUtils> f = std::make_shared<TFileUtils>();
	std::filesystem::path matp = m_path;
	std::filesystem::path testPath = std::filesystem::current_path();
	bool b = std::filesystem::exists(matp);

	YAML::Node node;
	YAML::Emitter emitter;
	emitter << YAML::BeginDoc;
	emitter << YAML::BeginMap;

	if (m_baseMap)
		emitter << YAML::Key << "baseMap" << YAML::Value << m_baseMap->m_path.generic_string();
	if (m_normalMap)
		emitter << YAML::Key << "normalMap" << YAML::Value << m_normalMap->m_path.generic_string();
	if (m_maskMap)
		emitter << YAML::Key << "maskMap" << YAML::Value << m_maskMap->m_path.generic_string();

	emitter << YAML::Key << "tileX" << YAML::Value << m_tileX;
	emitter << YAML::Key << "tileY" << YAML::Value << m_tileY;
	emitter << YAML::Key << "alphaCulling" << YAML::Value << m_alphaCulling;
	emitter << YAML::Key << "transparent" << YAML::Value << m_transparent;
	emitter << YAML::Key << "layer" << YAML::Value << m_layer;

	emitter << YAML::EndMap;
	emitter << YAML::EndDoc;

	std::ofstream fout(matp);
	fout << emitter.c_str();

	fout.close();
}