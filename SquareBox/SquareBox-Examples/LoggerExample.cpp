#include "LoggerExample.h"

void SquareBox::Example::LoggerExample::init()
{
	SquareBox::Log::Init();//initialize the logger , in our apps this is done by the GameScreen Mechanism Automatically
}

void SquareBox::Example::LoggerExample::onUpdate()
{
	SBX_CORE_INFO("INFO");
	SBX_CORE_CRITICAL("CRITICAL");
	SBX_CORE_ERROR("ERROR");
	SBX_CORE_TRACE("TRACE");
	SBX_CORE_WARN("WARN");

	SBX_INFO("INFO");
	SBX_CRITICAL("CRITICAL");
	SBX_ERROR("ERROR");
	SBX_TRACE("TRACE");
	SBX_WARN("WARN");
}

void SquareBox::Example::LoggerExample::dispose()
{
}