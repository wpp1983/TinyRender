#include "TinyRender.h"
#include "TinyRender_p.h"
#include "platform.h"
#include "defines.h"


namespace TinyRender
{
    static Context* s_ctx = nullptr;

    RenderFrame::Enum renderFrame(int32_t _msecs)
    {
        return RenderFrame::NoContext;
    }

    bool Init(InitParams const& _init)
    {
		if (nullptr != s_ctx)
		{
			BX_TRACE("TinyRender is already initialized.");
			return false;
		}

        BX_TRACE("Init...");

        s_ctx = new Context();

        if (s_ctx->Init(_init) )
		{
			BX_TRACE("Init complete.");
			return true;
		}

        BX_TRACE("Init failed.");

        return false;
    }


    bool Context::Init(const InitParams& _init)
    {
        if (m_rendererInitialized)
		{
			BX_TRACE("Already initialized!");
			return false;
		}

        m_init = _init;

        m_exit    = false;
		m_flipped = true;

        return true;
    }   

    void trace(const char* _filePath, uint16_t _line, const char* _format, ...)
    {

    }

    void fatal(const char* _filePath, uint16_t _line, Fatal::Enum _code, const char* _format, ...)
    {

    }

    PlatformData::PlatformData()
    	: ndt(nullptr)
		, nwh(nullptr)
		, context(nullptr)
		, backBuffer(nullptr)
		, backBufferDS(nullptr)
		, type(NativeWindowHandleType::Default)
    {
        
    }

    Resolution::Resolution()
    	: format(TextureFormat::RGBA8)
		, width(1280)
		, height(720)
		, reset(0)
		, numBackBuffers(2)
		, maxFrameLatency(0)
		, debugTextScale(0)
    {
        
    }


    InitParams::InitParams()
    	: type(RendererType::Count)
		, vendorId(BGFX_PCI_ID_NONE)
		, deviceId(0)
		, capabilities(UINT64_MAX)
		, debug(false)
		, profile(false)
    {
        
    }

    VertexLayout::VertexLayout()
    : m_stride(0)
    {

    }
    
    
    void SetViewClear(ViewId _id, uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil)
    {
        s_ctx->setViewClear(_id, _flags, _rgba, _depth, _stencil);
    }

    VertexBufferHandle createVertexBuffer(
		  const Memory* _mem
		, const VertexLayout& _layout
		, uint16_t _flags
		)
    {
		return s_ctx->createVertexBuffer(_mem, _layout, _flags);
    }
}
