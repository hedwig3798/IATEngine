#include "ClassDataSaver.h"
#include <yaml-cpp/yaml.h>

void Saver::SaveStart()
{

}

void Saver::SaveEnd()
{

}

template<>
void Saver::Save(int _val, const std::string& _name)
{

}

template<>
void Saver::Save(float _val, const std::string& _name)
{

}

template<>
void Saver::Save(bool _val, const std::string& _name)
{

}

template<>
void Saver::Save(std::string _val, const std::string& _name)
{

}

template<>
void Saver::Save(std::wstring _val, const std::string& _name)
{

}

template<>
void Saver::Save(Vector4 _val, const std::string& _name)
{

}

template<>
void Saver::Save(Vector3 _val, const std::string& _name)
{

}


template<>
void Saver::Save(Vector2 _val, const std::string& _name)
{

}
