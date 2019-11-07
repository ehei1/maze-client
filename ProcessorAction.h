#pragma once


#include "Processor.h"


class ProcessorAction : public Processor
{
public:
	ProcessorAction();
	virtual ~ProcessorAction();
	
	virtual bool Receive(const Event&, Body&);
};