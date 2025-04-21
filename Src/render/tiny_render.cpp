#include <bx/platform.h>

#include "tiny_render_p.h"

namespace TinyRender
{

    RendererType::Enum getRendererType()
    {
        return RendererType::Direct3D12;
    }

    void dump(const VertexLayout& _layout)
	{
		// if (BX_ENABLED(BGFX_CONFIG_DEBUG) )
		// {
		// 	BX_TRACE("VertexLayout %08x (%08x), stride %d"
		// 		, _layout.m_hash
		// 		, bx::hash<bx::HashMurmur2A>(_layout.m_attributes)
		// 		, _layout.m_stride
		// 		);

		// 	for (uint32_t attr = 0; attr < Attrib::Count; ++attr)
		// 	{
		// 		if (UINT16_MAX != _layout.m_attributes[attr])
		// 		{
		// 			uint8_t num;
		// 			AttribType::Enum type;
		// 			bool normalized;
		// 			bool asInt;
		// 			_layout.decode(Attrib::Enum(attr), num, type, normalized, asInt);

		// 			BX_TRACE("\tattr %2d: %-20s num %d, type %d, norm [%c], asint [%c], offset %2d"
		// 				, attr
		// 				, getAttribName(Attrib::Enum(attr) )
		// 				, num
		// 				, type
		// 				, normalized ? 'x' : ' '
		// 				, asInt      ? 'x' : ' '
		// 				, _layout.m_offset[attr]
		// 				);
		// 		}
		// 	}
		// }
	}


    	///
	RendererContextI* RendererCreate(const InitParams& _init);

	///
	void RendererDestroy(RendererContextI* _renderCtx);

    static Context* s_ctx = nullptr;

    void init(const struct InitParams &params) 
    {
        s_ctx = new Context();
        s_ctx->init(params);
    }

    VertexBufferHandle createVertexBuffer(const void* _data, uint32_t _size, const VertexLayout& _layout, uint16_t _flags)
    {
        BX_ASSERT(NULL != _data, "_data can't be NULL");
        BX_ASSERT(isValid(_layout), "Invalid VertexLayout.");

        return s_ctx->createVertexBuffer(_data, _size, _layout, _flags);
    }

    IndexBufferHandle createIndexBuffer(const void* _data, uint32_t _size, uint16_t _flags)
    {
        BX_ASSERT(NULL != _data, "_data can't be NULL");

        return s_ctx->createIndexBuffer(_data, _size, _flags);
    }

    ShaderHandle createShader(const void* _data, uint32_t _size, ShaderType _type)
    {
        BX_ASSERT(NULL != _data, "_data can't be NULL");

        return s_ctx->createShader(_data, _size, _type);
    }

    ProgramHandle createProgram(ShaderHandle _vsh, ShaderHandle _fsh)
    {
        BX_ASSERT(isValid(_vsh), "_vsh can't be NULL");
        BX_ASSERT(isValid(_fsh), "_fsh can't be NULL");

        return s_ctx->createProgram(_vsh, _fsh);
    }

    PSOHandle createPSO(ProgramHandle _program, const VertexLayout &_layout, uint16_t _flags)
    {
        BX_ASSERT(isValid(_program), "_program can't be NULL");
        BX_ASSERT(isValid(_layout), "_layout can't be NULL");

        return s_ctx->createPSO(_program, _layout, _flags);
    }

    void setViewTransform(ViewId _id, const void* _view, const void* _proj)
    {
        s_ctx->setViewTransform(_id, _view, _proj);
    }

    void setViewRect(ViewId _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
    {
        s_ctx->setViewRect(_id, _x, _y, _width, _height);
    }

    void beginFrame(ViewId _id)
    {
        s_ctx->beginFrame(_id);
    }

    void endFrame()
    {
        s_ctx->endFrame();
    }
    
    void drawMesh(VertexBufferHandle _vbh, IndexBufferHandle _ibh, ProgramHandle _program, PSOHandle _pso, uint16_t _state, const void* _mtx)
    {
        s_ctx->drawMesh(_vbh, _ibh, _program, _pso, _state, _mtx);
    }

    bool Context::init(const InitParams &_init)
    {
        m_renderCtx = RendererCreate(_init);

        return true;
    }

    void Context::shutdown()
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