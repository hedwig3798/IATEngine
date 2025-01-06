#pragma once
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace Truth
{
	class Component;
}

namespace Truth
{
	class ComponentFactory
	{
		using CreateFunction = std::function<std::shared_ptr<Truth::Component>(void)>;

	private:
		std::unordered_map<std::string, CreateFunction> m_creators;

	public:
		std::vector<const char*> m_componentList;

	public:
		ComponentFactory();
		// ���丮�� ��ü ���� �Լ��� ���
		void RegisterType(const char * typeName, CreateFunction createFunc) 
		{
			std::string name = typeName;
			m_creators[name] = createFunc;
			m_componentList.push_back(typeName);
		}

		// Ÿ�� �̸��� �������� ��ü�� ����
		std::shared_ptr<Component> Create(const std::string& typeName) const 
		{
			auto it = m_creators.find(typeName);
			if (it != m_creators.end()) {
				return it->second();
			}
			return nullptr;
		}
	};
}

