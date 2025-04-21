#pragma once

#define USE_D3D12_DYNAMIC_LIB (BX_PLATFORM_WINDOWS || BX_PLATFORM_LINUX)

#include <sal.h>
#include <unknwn.h>

#if BX_PLATFORM_LINUX || BX_PLATFORM_WINDOWS || BX_PLATFORM_WINRT
#include <d3d12.h>
#else
#if !BGFX_CONFIG_DEBUG
#define D3DCOMPILE_NO_DEBUG 1
#endif // !BGFX_CONFIG_DEBUG
#include <d3d12_x.h>
#endif // BX_PLATFORM_XBOXONE

#if defined(__MINGW32__) // BK - temp workaround for MinGW until I nuke d3dx12 usage.
extern "C++"
{
#if defined(__cpp_constexpr) && __cpp_constexpr >= 200704L && defined(__cpp_inline_variables) &&                       \
    __cpp_inline_variables >= 201606L
    __extension__ template <typename Ty> constexpr const GUID &__mingw_uuidof();
#else
    __extension__ template <typename Ty> const GUID &__mingw_uuidof();
#endif // __cpp_*

    template <> const GUID &__mingw_uuidof<ID3D12Device>()
    {
        static const GUID IID_ID3D12Device0 = {
            0x189819f1, 0x1db6, 0x4b57, {0xbe, 0x54, 0x18, 0x21, 0x33, 0x9b, 0x85, 0xf7}};
        return IID_ID3D12Device0;
    }
}
#endif // defined(__MINGW32__)

#define DX_CHECK_EXTRA_F ""
#define DX_CHECK_EXTRA_ARGS

#define _DX_CHECK(_call)                                                                                               \
    BX_MACRO_BLOCK_BEGIN                                                                                               \
    HRESULT __hr__ = _call;                                                                                            \
    BX_ASSERT(SUCCEEDED(__hr__), #_call " FAILED 0x%08x" DX_CHECK_EXTRA_F "\n", (uint32_t)__hr__ DX_CHECK_EXTRA_ARGS); \
    BX_MACRO_BLOCK_END

#if BGFX_CONFIG_DEBUG
#define DX_CHECK(_call) _DX_CHECK(_call)
#define DX_CHECK_REFCOUNT(_ptr, _expected) _DX_CHECK_REFCOUNT(_ptr, _expected)
#else
#define DX_CHECK(_call) _call
#define DX_CHECK_REFCOUNT(_ptr, _expected)
#endif // BGFX_CONFIG_DEBUG

#include "rhi.h"

namespace TinyRender
{
namespace d3d12
{

struct BufferD3D12
{
    BufferD3D12()
        : m_ptr(NULL), m_state(D3D12_RESOURCE_STATE_COMMON), m_size(0), m_flags(BGFX_BUFFER_NONE), m_dynamic(false)
    {
    }

    void create(uint32_t _size, const void *_data, uint16_t _flags, bool _vertex, uint32_t _stride = 0);

    void update(ID3D12GraphicsCommandList *_commandList, uint32_t _offset, uint32_t _size, const void *_data,
                bool _discard = false);

    void destroy();

    D3D12_RESOURCE_STATES setState(ID3D12GraphicsCommandList *_commandList, D3D12_RESOURCE_STATES _state);

    D3D12_SHADER_RESOURCE_VIEW_DESC m_srvd;
    D3D12_UNORDERED_ACCESS_VIEW_DESC m_uavd;
    ID3D12Resource *m_ptr;
    D3D12_GPU_VIRTUAL_ADDRESS m_gpuVA;
    D3D12_RESOURCE_STATES m_state;
    uint32_t m_size;
    uint16_t m_flags;
    bool m_dynamic;
};

struct VertexBufferD3D12 : public BufferD3D12
{
    void create(uint32_t _size, const void *_data, VertexLayoutHandle _layoutHandle, uint16_t _flags);

    VertexLayoutHandle m_layoutHandle;
};

struct ShaderD3D12
{
    ShaderD3D12() : m_code(NULL), m_size(0) {}

    void create(const void* _data, uint32_t _size, ShaderType _type);

    void destroy()
    {
        if (NULL != m_code)
        {
            m_code = NULL;
            m_size = 0;
        }
    }

    const void *m_code;
	uint32_t m_size;
    // uint16_t m_attrMask[Attrib::Count];
	ShaderType m_type;
    ID3DBlob* m_shader;
    
};

struct ProgramD3D12
{
    ProgramD3D12() : m_vsh(NULL), m_fsh(NULL) {}

    void create(const ShaderD3D12 *_vsh, const ShaderD3D12 *_fsh);

    void destroy()
    {
        m_vsh = NULL;
        m_fsh = NULL;
    }

    const ShaderD3D12 *m_vsh;
    const ShaderD3D12 *m_fsh;
};

struct PSOD3D12
{
    PSOD3D12() : m_pso(NULL) {}

    void create(const ProgramD3D12 *_program, const VertexLayout *_layout, uint16_t _flags);
    void destroy()
    {
        m_pso = NULL;
    }

    ID3D12PipelineState *m_pso;
    
};

} // namespace d3d12
} // namespace TinyRender
