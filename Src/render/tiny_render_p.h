#pragma once

#include <bx/platform.h>

#include <bx/handlealloc.h>
#include <bx/math.h>
#include <bx/float4x4_t.h>
#include <bx/string.h>

#include "tiny_render.h"

#define BGFX_CONFIG_RENDERER_DIRECT3D12 1

#define BGFX_CONFIG_DEBUG BX_CONFIG_DEBUG

namespace TinyRender
{

inline bool isValid(const VertexLayout &_layout)
{
    return 0 != _layout.m_stride;
}

BX_ALIGN_DECL_16(struct) Matrix4
{
    union
    {
        float val[16];
        bx::float4x4_t f4x4;
    } un;

    void setIdentity()
    {
        bx::memSet(un.val, 0, sizeof(un.val));
        un.val[0] = un.val[5] = un.val[10] = un.val[15] = 1.0f;
    }
};

struct Rect
{
    Rect() {}

    Rect(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
        : m_x(_x), m_y(_y), m_width(_width), m_height(_height)
    {
    }

    void clear()
    {
        m_x = 0;
        m_y = 0;
        m_width = 0;
        m_height = 0;
    }


    void set(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
    {
        m_x = _x;
        m_y = _y;
        m_width = _width;
        m_height = _height;
    }

    uint16_t m_x;
    uint16_t m_y;
    uint16_t m_width;
    uint16_t m_height;
};

BX_ALIGN_DECL_CACHE_LINE(struct) View
{
    void reset()
    {
        setRect(0, 0, 1, 1);
        setTransform(NULL, NULL);
    }

    void setRect(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
    {
        m_rect.m_x = _x;
        m_rect.m_y = _y;
        m_rect.m_width = _width;
        m_rect.m_height = _height;
    }


    void setTransform(const void *_view, const void *_proj)
    {
        if (NULL != _view)
        {
            bx::memCopy(m_view.un.val, _view, sizeof(Matrix4));
        }
        else
        {
            m_view.setIdentity();
        }

        if (NULL != _proj)
        {
            bx::memCopy(m_proj.un.val, _proj, sizeof(Matrix4));
        }
        else
        {
            m_proj.setIdentity();
        }
    }

    Rect m_rect;
    Matrix4 m_view;
    Matrix4 m_proj;
};


struct BX_NO_VTABLE RendererContextI
{
    virtual ~RendererContextI() = 0;
    virtual void createIndexBuffer(IndexBufferHandle _handle, const void *_data, uint32_t _size, uint16_t _flags) = 0;
    virtual void destroyIndexBuffer(IndexBufferHandle _handle) = 0;
    virtual void createVertexLayout(VertexLayoutHandle _handle, const VertexLayout &_layout) = 0;
    virtual void destroyVertexLayout(VertexLayoutHandle _handle) = 0;
    virtual void createVertexBuffer(VertexBufferHandle _handle, const void *_data, uint32_t _size,
                                    VertexLayoutHandle _layoutHandle, uint16_t _flags) = 0;
    virtual void destroyVertexBuffer(VertexBufferHandle _handle) = 0;
    virtual void createShader(ShaderHandle _handle, const void *_data, uint32_t _size, ShaderType _type) = 0;
    virtual void createProgram(ProgramHandle _handle, ShaderHandle _vsh, ShaderHandle _fsh) = 0;
    virtual void createPSO(PSOHandle _handle, ProgramHandle _program, const VertexLayout &_layout, uint16_t _flags) = 0;
    virtual void beginFrame(const View &_view) = 0;
    virtual void endFrame() = 0;
    virtual void drawMesh(VertexBufferHandle _vbh, IndexBufferHandle _ibh, ProgramHandle _program, PSOHandle _pso, uint16_t _state, const void* _mtx) = 0;
};

inline RendererContextI::~RendererContextI() {}

#if BGFX_CONFIG_DEBUG
#define BGFX_API_FUNC(_func) BX_NO_INLINE _func
#else
#define BGFX_API_FUNC(_func) _func
#endif // BGFX_CONFIG_DEBUG

/// Dump vertex layout info into debug output.
void dump(const VertexLayout &_layout);

const char *getAttribName(Attrib::Enum _attr);





struct Context
{
    bool init(const InitParams &_init);
    void shutdown();

    VertexLayoutHandle findOrCreateVertexLayout(const VertexLayout &_layout)
    {

        VertexLayoutHandle layoutHandle = {m_layoutHandle.alloc()};
        if (!isValid(layoutHandle))
        {
            BX_TRACE("WARNING: Failed to allocate vertex layout handle (BGFX_CONFIG_MAX_VERTEX_LAYOUTS, max: %d).",
                     BGFX_CONFIG_MAX_VERTEX_LAYOUTS);
            return BGFX_INVALID_HANDLE;
        }

        m_renderCtx->createVertexLayout(layoutHandle, _layout);

        return layoutHandle;
    }

    BGFX_API_FUNC(VertexBufferHandle createVertexBuffer(const void *_data, uint32_t _size, const VertexLayout &_layout,
                                                        uint16_t _flags))
    {
        VertexBufferHandle handle = {m_vertexBufferHandle.alloc()};
        if (isValid(handle))
        {
            VertexLayoutHandle layoutHandle = findOrCreateVertexLayout(_layout);
            if (!isValid(layoutHandle))
            {
                BX_TRACE("WARNING: Failed to allocate vertex layout handle (BGFX_CONFIG_MAX_VERTEX_LAYOUTS, max: %d).",
                         BGFX_CONFIG_MAX_VERTEX_LAYOUTS);
                m_vertexBufferHandle.free(handle.idx);
                return BGFX_INVALID_HANDLE;
            }
            m_renderCtx->createVertexBuffer(handle, _data, _size, layoutHandle, _flags);
        }
        return handle;
    }

    BGFX_API_FUNC(IndexBufferHandle createIndexBuffer(const void *_data, uint32_t _size, uint16_t _flags))
    {
        IndexBufferHandle handle = {m_indexBufferHandle.alloc()};
        BX_WARN(isValid(handle), "Failed to allocate index buffer handle.");
        if (isValid(handle))
        {
            m_renderCtx->createIndexBuffer(handle, _data, _size, _flags);
        }
        return handle;
    }

    BGFX_API_FUNC(ShaderHandle createShader(const void *_data, uint32_t _size, ShaderType _type))
    {
        ShaderHandle handle = {m_shaderHandle.alloc()};
        BX_WARN(isValid(handle), "Failed to allocate shader handle.");
        if (isValid(handle))
        {
            m_renderCtx->createShader(handle, _data, _size, _type);
        }
        return handle;
    }

    BGFX_API_FUNC(ProgramHandle createProgram(ShaderHandle _vsh, ShaderHandle _fsh))
    {
        ProgramHandle handle = {m_programHandle.alloc()};
        BX_WARN(isValid(handle), "Failed to allocate program handle.");
        if (isValid(handle))
        {
            m_renderCtx->createProgram(handle, _vsh, _fsh);
        }
        return handle;
    }

    BGFX_API_FUNC(PSOHandle createPSO(ProgramHandle _program, const VertexLayout &_layout, uint16_t _flags))
    {
        PSOHandle handle = {m_psoHandle.alloc()};
        BX_WARN(isValid(handle), "Failed to allocate pso handle.");
        if (isValid(handle))
        {   
            m_renderCtx->createPSO(handle, _program, _layout, _flags);
        }
        return handle;
    }

    BGFX_API_FUNC(void setViewTransform(ViewId _id, const void *_view, const void *_proj))
    {
        m_view[_id].setTransform(_view, _proj);
    }

    BGFX_API_FUNC(void setViewRect(ViewId _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height))
    {
        m_view[_id].setRect(_x, _y, _width, _height);
    }

    BGFX_API_FUNC(void beginFrame(ViewId _id))
    {
        m_renderCtx->beginFrame(m_view[_id]);
    }

    BGFX_API_FUNC(void endFrame())
    {
        m_renderCtx->endFrame();
    }
    

    BGFX_API_FUNC(void drawMesh(VertexBufferHandle _vbh, IndexBufferHandle _ibh, ProgramHandle _program, PSOHandle _pso, uint16_t _state, const void* _mtx))
    {
        m_renderCtx->drawMesh(_vbh, _ibh, _program, _pso, _state, _mtx);
    }

    RendererContextI *m_renderCtx;

    bx::HandleAllocT<BGFX_CONFIG_MAX_INDEX_BUFFERS> m_indexBufferHandle;
    bx::HandleAllocT<BGFX_CONFIG_MAX_VERTEX_LAYOUTS> m_layoutHandle;

    bx::HandleAllocT<BGFX_CONFIG_MAX_VERTEX_BUFFERS> m_vertexBufferHandle;
    bx::HandleAllocT<BGFX_CONFIG_MAX_SHADERS> m_shaderHandle;
    bx::HandleAllocT<BGFX_CONFIG_MAX_PROGRAMS> m_programHandle;
    bx::HandleAllocT<BGFX_CONFIG_MAX_PSOS> m_psoHandle;
    // bx::HandleAllocT<BGFX_CONFIG_MAX_TEXTURES> m_textureHandle;
    // bx::HandleAllocT<BGFX_CONFIG_MAX_FRAME_BUFFERS> m_frameBufferHandle;
    // bx::HandleAllocT<BGFX_CONFIG_MAX_UNIFORMS> m_uniformHandle;
    // bx::HandleAllocT<BGFX_CONFIG_MAX_OCCLUSION_QUERIES> m_occlusionQueryHandle;

    View m_view[BGFX_CONFIG_MAX_VIEWS];
};

} // namespace TinyRender
