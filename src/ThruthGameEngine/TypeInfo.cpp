#include "TypeInfo.h"
#include "Method.h"
#include "Property.h"

std::unique_ptr<Truth::ComponentFactory> TypeInfo::g_componentFactory;
std::unique_ptr<Truth::ButtonFactory> TypeInfo::g_buttonFactory;

const TypeInfo* TypeInfo::GetSuper() const
{
	return m_super;
}

bool TypeInfo::IsA(const TypeInfo& other) const
{
	if (this == &other)
	{
		return true;
	}

	return m_typeHash == other.m_typeHash;
}

bool TypeInfo::IsChildOf(const TypeInfo& other) const
{
	if (IsA(other))
	{
		return true;
	}

	for (const TypeInfo* super = m_super; super != nullptr; super = super->GetSuper())
	{
		if (super->IsA(other))
		{
			return true;
		}
	}

	return false;
}

const Method* TypeInfo::GetMethod(const char* name) const
{
	auto iter = m_methodMap.find(name);
	return (iter != std::end(m_methodMap)) ? iter->second : nullptr;
}

const Property* TypeInfo::GetProperty(const char* name) const
{
	auto iter = m_propertyMap.find(name);
	return (iter != std::end(m_propertyMap)) ? iter->second : nullptr;
}

void TypeInfo::CollectSuperMethods()
{
	const std::vector<const Method*>& methods = m_super->GetMethods();
	for (const Method* method : methods)
	{
		AddMethod(method);
	}
}

void TypeInfo::CollectSuperProperties()
{
	const std::vector<const Property*>& properties = m_super->GetProperties();
	for (const Property* property : properties)
	{
		AddProperty(property);
	}
}

void TypeInfo::AddProperty(const Property* property)
{
	m_properties.emplace_back(property);
	m_propertyMap.emplace(property->GetName(), property);
}

void TypeInfo::AddMethod(const Method* method)
{
	m_methods.emplace_back(method);
	m_methodMap.emplace(method->GetName(), method);
}

std::string TypeInfo::Dump(void* _object, int _indent) const
{
	std::string result;
	result += m_name;
	result += "\n";

	result += std::string(_indent, '\t');
	result += "{\n";

	result += std::string(_indent, '\t');
	result += "Property : \n";
	for (auto& p : m_properties)
	{
		result += p->Dump(_object, _indent + 1);
	}

	result += std::string(_indent, '\t');
	result += "Function : \n";
	for (auto& m : m_methods)
	{
		result += m->Dump(_object, _indent + 1);
	}

	result += std::string(_indent, '\t');
	result += "}\n";

	return result;
}
