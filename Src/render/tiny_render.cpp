#include "tiny_render_p.h"

namespace TinyRender
{

    	///
	RendererContextI* RendererCreate(const InitParams& _init);

	///
	void RendererDestroy(RendererContextI* _renderCtx);

    static Context* s_ctx = nullptr;

    void Init(const struct InitParams &params) 
    {
        s_ctx = new Context();
        s_ctx->Init(params);
    }

    bool Context::Init(const InitParams &_init)
    {
        m_renderCtx = RendererCreate(_init);

        return true;
    }

    void Context::Shutdown()
    {
        RendererDestroy(m_renderCtx);
        m_renderCtx = nullptr;
    }


	typedef RendererContextI* (*RendererCreateFn)(const InitParams& _init);
	typedef void (*RendererDestroyFn)();

#define BGFX_RENDERER_CONTEXT(_namespace)                           \
	namespace _namespace                                            \
	{                                                               \
		extern RendererContextI* rendererCreate(const InitParams& _init); \
		extern void rendererDestroy();                              \
	}

	BGFX_RENDERER_CONTEXT(d3d12);

#undef BGFX_RENDERER_CONTEXT

    RendererContextI* RendererCreate(const InitParams& _init)
    {
        #if BGFX_CONFIG_RENDERER_DIRECT3D12
            return d3d12::rendererCreate(_init);
        #endif
        return nullptr;
    }

    void RendererDestroy(RendererContextI* _renderCtx)
    {
        #if BGFX_CONFIG_RENDERER_DIRECT3D12
            d3d12::rendererDestroy();
        #endif
    }

} // namespace TinyRender