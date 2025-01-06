#pragma once
/// 매크로 함수를 모아둔 헤더

#define SERIALIZE_CUSTOM() \
private: \
friend class boost::serialization::access; \
BOOST_SERIALIZATION_SPLIT_MEMBER(); \
template<class Archive> \
void save(Archive& _ar, const unsigned int _file_version) const; \
template<class Archive> \
void load(Archive& _ar, const unsigned int _file_version); \