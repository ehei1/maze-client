#include "StdAfx.h"
#include "Command.h"
#include "Event.h"


Command::Command(const Event& event) :
mType(event.GetType())
{}


Command::~Command(void)
{
}
