#pragma once
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#include "EditorDefine.h"
// #define yaml_cpp_EXPORTS
// extern library
#include "SimpleMath.h"
#include "Types.h"
#include "Logger.h"
using namespace DirectX::SimpleMath;

// window
#include <windows.h>

// memory
#include <memory>

// data type
#include <string>

// data structer
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <queue>
#include <set>

// type trait
#include <type_traits>
#include <typeinfo>
// #include <any>

// assert
#include <assert.h>

// �Լ� ����
#include <functional>

// ��ũ�� ����
#include "Macro.h"

// ���÷��� ����
#include "AdvanceReflection.h"

// �׷��� ����
#include "ICamera.h"
#include "IdealRendererFactory.h"
#include "IdealRenderer.h"
#include "IMeshObject.h"
#include "IDirectionalLight.h"
#include "ISpotLight.h"
#include "IPointLight.h"

// ����ȭ ����
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/weak_ptr.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <stdio.h>
#include <sstream>
#include <fstream>

#include <locale>
#include <codecvt>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;
#include <atlconv.h>