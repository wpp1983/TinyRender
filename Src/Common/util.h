#pragma once

#include "Render/TinyRender.h"

struct Args
{
	Args(int _argc, const char* const* _argv);

	TinyRender::RendererType::Enum m_type;
	uint16_t m_pciId;
};


