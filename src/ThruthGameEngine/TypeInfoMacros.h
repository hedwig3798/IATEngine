#pragma once

#include "TypeInfo.h"

template <typename Str>
constexpr size_t HashString(const Str& toHash)
{
	// 	// For this example, I'm requiring size_t to be 64-bit, but you could
	// 	// easily change the offset and prime used to the appropriate ones
	// 	// based on sizeof(size_t).
	// 	static_assert(sizeof(size_t) == 8);
	// 	// FNV-1a 64 bit algorithm
	// 	size_t result = 0xcbf29ce484222325; // FNV offset basis
	// 
	// 	for (char c : toHash) {
	// 		result ^= c;
	// 		result *= 1099511628211; // FNV prime
	// 	}
	std::hash<std::string> str_hash;
	return str_hash(toHash);
}

#define GENERATE_CLASS_TYPE_INFO(TypeName)	\
public: \
	inline static size_t m_typeID = HashString(#TypeName); \
private: \
	friend SuperClassTypeDeduction<TypeName>; \
	friend TypeInfoInitializer<TypeName>; \
\
public:	\
	using Super = typename SuperClassTypeDeduction<TypeName>::Type; \
	using ThisType = TypeName; \
\
	static TypeInfo& StaticTypeInfo() \
	{ \
		static TypeInfo typeInfo{ TypeInfoInitializer<ThisType>( #TypeName ) }; \
		return typeInfo; \
	} \
\
	virtual const TypeInfo& GetTypeInfo() const \
	{ \
		return m_typeInfo; \
	} \
\
private: \
	inline static TypeInfo& m_typeInfo = StaticTypeInfo(); \
\
private: