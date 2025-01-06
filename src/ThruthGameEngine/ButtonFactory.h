#pragma once
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace Truth
{
	class ButtonBehavior;
}

namespace Truth
{
	class ButtonFactory
	{
		using CreateFunction = std::function<std::shared_ptr<Truth::ButtonBehavior>(void)>;

	private:
		std::unordered_map<std::string, CreateFunction> m_creators;

	public:
		std::vector<const char*> m_buttonList;

	public:
		ButtonFactory();
		// ���丮�� ��ü ���� �Լ��� ���
		void RegisterType(const char* typeName, CreateFunction createFunc)
		{
			std::string name = typeName;
			m_creators[name] = createFunc;
			m_buttonList.push_back(typeName);
		}

		// Ÿ�� �̸��� �������� ��ü�� ����
		std::shared_ptr<ButtonBehavior> Create(const std::string& typeName) const
		{
			auto it = m_creators.find(typeName);
			if (it != m_creators.end()) {
				return it->second();
			}
			return nullptr;
		}
	};
}

