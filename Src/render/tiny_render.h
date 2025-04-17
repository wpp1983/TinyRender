#pragma once

#include <stdarg.h> // va_list
#include <stdint.h> // uint32_t
#include <stdlib.h> // NULL

///
#define BGFX_HANDLE(_name)                                                                                             \
    struct _name                                                                                                       \
    {                                                                                                                  \
        uint16_t idx;                                                                                                  \
    };                                                                                                                 \
    inline bool isValid(_name _handle)                                                                                 \
    {                                                                                                                  \
        return TinyRender::kInvalidHandle != _handle.idx;                                                              \
    }

#define BGFX_INVALID_HANDLE {TinyRender::kInvalidHandle}

namespace TinyRender
{

    static const uint16_t kInvalidHandle = UINT16_MAX;

	BGFX_HANDLE(DynamicIndexBufferHandle)
	BGFX_HANDLE(DynamicVertexBufferHandle)
	BGFX_HANDLE(FrameBufferHandle)
	BGFX_HANDLE(IndexBufferHandle)
	BGFX_HANDLE(IndirectBufferHandle)
	BGFX_HANDLE(OcclusionQueryHandle)
	BGFX_HANDLE(ProgramHandle)
	BGFX_HANDLE(ShaderHandle)
	BGFX_HANDLE(TextureHandle)
	BGFX_HANDLE(UniformHandle)
	BGFX_HANDLE(VertexBufferHandle)
	BGFX_HANDLE(VertexLayoutHandle)


	struct VertexLayout
	{


	};

struct InitParams
{
    int width;
    int height;
    int samples;
    int maxDepth;
    void* hwnd;
};

void Init(const InitParams &params);

} // namespace TinyRender
