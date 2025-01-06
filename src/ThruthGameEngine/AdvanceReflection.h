#pragma once

#include "MethodMacros.h"
#include "PropertyMacros.h"
#include "TypeInfoMacros.h"

template <typename To, typename From>
To* Cast(From* src)
{
	return src->GetTypeInfo().IsChildOf<To>() ? reinterpret_cast<To*>(src) : nullptr;
}

template <typename To, typename From>
std::shared_ptr<To> Cast(std::shared_ptr<From> src)
{
	return src->GetTypeInfo().IsChildOf<To>() ? std::reinterpret_pointer_cast<To>(src) : nullptr;
}