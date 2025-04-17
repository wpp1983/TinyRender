#pragma once

#include <bx/platform.h>
#include <bx/math.h>
#include <bx/allocator.h>
#include <bx/thread.h>
#include <bx/debug.h>
#include <bx/commandline.h>
#include <bx/handlealloc.h>



#include "tiny_render.h"

#define BGFX_CONFIG_RENDERER_DIRECT3D12 1

#define BGFX_CONFIG_DEBUG BX_CONFIG_DEBUG

namespace TinyRender
{
    struct BX_NO_VTABLE RendererContextI
	{
        virtual ~RendererContextI() = 0;
		virtual void createIndexBuffer(IndexBufferHandle _handle, const void* _mem, uint16_t _flags) = 0;
		virtual void destroyIndexBuffer(IndexBufferHandle _handle) = 0;
		virtual void createVertexLayout(VertexLayoutHandle _handle, const VertexLayout& _layout) = 0;
		virtual void destroyVertexLayout(VertexLayoutHandle _handle) = 0;
		virtual void createVertexBuffer(VertexBufferHandle _handle, const void* _mem, VertexLayoutHandle _layoutHandle, uint16_t _flags) = 0;
		virtual void destroyVertexBuffer(VertexBufferHandle _handle) = 0;
    };

    inline RendererContextI::~RendererContextI()
	{
	}


    struct Context
    {
        bool Init(const InitParams &_init);
        void Shutdown();

        RendererContextI* m_renderCtx;
    };



} // namespace TinyRender
