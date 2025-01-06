#pragma once

namespace Ideal
{
	class ITexture
	{
	public:
		ITexture() {}
		virtual ~ITexture() {}

	public:
		// ���� : ImGui::Image((ImTextureID)GetImageID(), ImVec2) -> ImVec2�� �̹��� ������
		virtual unsigned long long GetImageID() abstract;

	public:
		virtual unsigned int GetWidth() abstract;
		virtual unsigned int GetHeight() abstract;

	private:
	};
}