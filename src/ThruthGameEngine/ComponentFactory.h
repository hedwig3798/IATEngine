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
		// 팩토리에 객체 생성 함수를 등록
		void RegisterType(const char * typeName, CreateFunction createFunc) 
		{
			std::string name = typeName;
			m_creators[name] = createFunc;
			m_componentList.push_back(typeName);
		}

		// 타입 이름을 바탕으로 객체를 생성
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

