#pragma once
#include <string>
#include <type_traits>
#include <memory>
#include <vector>
#include <map>
#include <list>
#include "SimpleMath.h"
#include "imgui.h"
#include <atlconv.h>
#include "Material.h"
#include "Texture.h"
#include "ITexture.h"
#include "IMaterial.h"
#include "ISprite.h"
#include "Material.h"
#include "Texture.h"
// #include "ButtonBehavior.h"
#include "TypeInfo.h"
#include "UISpriteSet.h"

#define PI 3.1415926
#define RadToDeg 57.29577951f
#define DegToRad 0.017453293f

using namespace DirectX::SimpleMath;

namespace Truth
{
	class Entity;
	class Component;
	class Scene;
	class ButtonBehavior;
}
#pragma warning(disable : 6387)
#pragma warning(disable : 6255)
namespace TypeUI
{
	// 템플릿 타입에 대한 타입은 여기서 정의한다.
	template<typename T>
	bool DisplayType(T& _val, const char* _name, float _min = 0.0f, float _max = 0.0f, uint32 _index = 0)
	{
		if constexpr (std::is_same_v<T, int>)
		{
			return ImGui::DragInt(_name, &_val, 1.0f, (int)_min, (int)_max);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return ImGui::Checkbox(_name, &_val);
		}
		else if constexpr (std::is_same_v<T, unsigned int>)
		{
			return ImGui::DragScalar(_name, ImGuiDataType_::ImGuiDataType_U64, &_val);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			return ImGui::DragFloat(_name, &_val, 0.01f, _min, _max);
		}
		else if constexpr (std::is_same_v<T, Vector3>)
		{
			float value[3] = {};
			value[0] = _val.x;
			value[1] = _val.y;
			value[2] = _val.z;
			bool isSelect = ImGui::DragFloat3(_name, value, 0.01f);
			if (isSelect)
			{
				_val.x = value[0];
				_val.y = value[1];
				_val.z = value[2];
			}
			return isSelect;
		}
		else if constexpr (std::is_same_v<T, Vector2>)
		{
			float value[3] = {};
			value[0] = _val.x;
			value[1] = _val.y;
			bool isSelect = ImGui::DragFloat2(_name, value, 0.01f);
			if (isSelect)
			{
				_val.x = value[0];
				_val.y = value[1];
			}
			return isSelect;
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			USES_CONVERSION;
			std::string sval(_val.c_str());

			sval.resize(128);

			bool success = ImGui::InputText(_name, (char*)sval.c_str(), 128, ImGuiInputTextFlags_EnterReturnsTrue);
			if (success)
			{
				_val = sval.c_str();
			}
			return success;
		}
		else if constexpr (std::is_same_v<T, std::wstring>)
		{
			USES_CONVERSION;
			std::string sval(W2A(_val.c_str()));

			sval.resize(128);

			bool success = ImGui::InputText(_name, (char*)sval.c_str(), 128, ImGuiInputTextFlags_EnterReturnsTrue);
			if (success)
			{
				_val = A2W(sval.c_str());
			}
			return success;
		}
		else if constexpr (std::is_same_v<T, Quaternion>)
		{
			float value[3] = {};

			Vector3 delta;

			Vector3 temp = _val.ToEuler();

			temp.x = temp.x * RadToDeg;
			temp.y = temp.y * RadToDeg;
			temp.z = temp.z * RadToDeg;

			value[0] = temp.x;
			value[1] = temp.y;
			value[2] = temp.z;

			ImGui::DragFloat3(_name, value, 0.1f);

			delta.x = value[0] * DegToRad;
			delta.y = value[1] * DegToRad;
			delta.z = value[2] * DegToRad;

			_val = Quaternion::CreateFromYawPitchRoll(delta);
			return false;
		}
		else if constexpr (std::is_same_v<T, Color>)
		{
			float value[4] = {};

			value[0] = _val.x;
			value[1] = _val.y;
			value[2] = _val.z;
			value[3] = _val.w;

			bool isSelect = ImGui::DragFloat4(_name, value, 0.001f, 0.0f, 1.0f);
			if (isSelect)
			{
				_val = {
					value[0],
					value[1],
					value[2],
					value[3]
				};
			}
			return isSelect;
		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<Truth::Material>>)
		{
			const ImVec2 size(100, 100);

			std::string fileID = _val->m_name + "##" + std::to_string(_index);
			if (ImGui::Button(fileID.c_str()))
			{
				// _val->ChangeMaterial();
			}
			ImGui::SameLine();
			if (ImGui::Button(("new##" + fileID).c_str()))
			{
				// _val->ChangeMaterial();
			}

			if (_val->m_baseMap != nullptr)
			{
				if (ImGui::ImageButton((fileID + "0").c_str(), (ImTextureID)(_val->m_baseMap->GetImageID()), size))
					_val->ChangeTexture(0);
			}
			ImGui::SameLine();
			if (_val->m_normalMap != nullptr)
			{
				if (ImGui::ImageButton((fileID + "1").c_str(), (ImTextureID)(_val->m_normalMap->GetImageID()), size))
					_val->ChangeTexture(1);
			}
			ImGui::SameLine();
			if (_val->m_maskMap != nullptr)
			{
				if (ImGui::ImageButton((fileID + "2").c_str(), (ImTextureID)(_val->m_maskMap->GetImageID()), size))
					_val->ChangeTexture(2);
			}
		}
// 		else if constexpr (std::is_same_v<T, std::shared_ptr<Truth::ButtonBehavior>>)
// 		{
// 			if (_val != nullptr)
// 				ImGui::Text(_val->m_name.c_str());
// 
// 			const auto& buttonList = TypeInfo::g_buttonFactory->m_buttonList;
// 			if (ImGui::CollapsingHeader("Set Button"))
// 			{
// 				int selectedItem = -1;
// 				if (ImGui::ListBox("Button", &selectedItem, buttonList.data(), static_cast<int32>(buttonList.size()), 6))
// 				{
// 					if (_val)
// 						_val.reset();
// 					_val = TypeInfo::g_buttonFactory->Create(buttonList[selectedItem]);
// 					return true;
// 				}
// 			}
// 			return false;
// 		}
		else if constexpr (std::is_same_v<T, std::shared_ptr<Truth::UISpriteSet>>)
		{
			const ImVec2 size(100, 100);
			std::string fileID = _val->m_name + "##" + std::to_string(_index);
			bool isSelect = false;
			if (_val->m_idealTexture != nullptr)
			{
				if (ImGui::ImageButton((fileID + "0").c_str(), (ImTextureID)(_val->m_idealTexture->GetImageID()), size))
				{
					isSelect |= true;
					_val->ChangeTexture(0);
				}
			}
			ImGui::SameLine();
			if (_val->m_overTexture != nullptr)
			{
				if (ImGui::ImageButton((fileID + "1").c_str(), (ImTextureID)(_val->m_overTexture->GetImageID()), size))
				{
					isSelect |= true;
					_val->ChangeTexture(1);
				}
			}
			ImGui::SameLine();
			if (_val->m_clickTexture != nullptr)
			{
				if (ImGui::ImageButton((fileID + "2").c_str(), (ImTextureID)(_val->m_clickTexture->GetImageID()), size))
				{
					isSelect |= true;
					_val->ChangeTexture(2);
				}
			}
			return isSelect;
		}
		return false;
	}
};

