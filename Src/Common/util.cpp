#include "util.h"
#include "defines.h"

Args::Args(int _argc, const char* const* _argv)
	: m_type(TinyRender::RendererType::Count)
	, m_pciId(BGFX_PCI_ID_NONE)
{

}

