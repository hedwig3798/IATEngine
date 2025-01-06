#pragma once
#include "Headers.h"
#include "Entity.h"

class EmptyEntity
	: public Truth::Entity
{
	GENERATE_CLASS_TYPE_INFO(EmptyEntity)

public:
	EmptyEntity();
	virtual ~EmptyEntity();
	virtual void Initailize() override;
};

