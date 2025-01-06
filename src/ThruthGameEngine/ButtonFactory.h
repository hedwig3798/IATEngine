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
		// 팩토리에 객체 생성 함수를 등록
		void RegisterType(const char* typeName, CreateFunction createFunc)
		{
			std::string name = typeName;
			m_creators[name] = createFunc;
			m_buttonList.push_back(typeName);
		}

		// 타입 이름을 바탕으로 객체를 생성
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

