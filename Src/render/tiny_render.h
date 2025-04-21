#pragma once

#include <stdarg.h>  // va_list
#include <stdbool.h> // bool
#include <stdint.h>  // uint32_t
#include <stdlib.h>  // size_t

#include <bx/platform.h>

#include "defines.h"

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

/// Renderer backend type enum.
///
/// @attention C99's equivalent binding is `bgfx_renderer_type_t`.
///
struct RendererType
{
    /// Renderer types:
    enum Enum
    {
        Noop,       //!< No rendering.
        Agc,        //!< AGC
        Direct3D11, //!< Direct3D 11.0
        Direct3D12, //!< Direct3D 12.0
        Gnm,        //!< GNM
        Metal,      //!< Metal
        Nvn,        //!< NVN
        OpenGLES,   //!< OpenGL ES 2.0+
        OpenGL,     //!< OpenGL 2.1+
        Vulkan,     //!< Vulkan

        Count
    };
};

RendererType::Enum getRendererType();

/// Vertex attribute enum.
///
/// @attention C99's equivalent binding is `bgfx_attrib_t`.
///
struct Attrib
{
    /// Corresponds to vertex shader attribute.
    enum Enum
    {
        Position,  //!< a_position
        Normal,    //!< a_normal
        Tangent,   //!< a_tangent
        Bitangent, //!< a_bitangent
        Color0,    //!< a_color0
        Color1,    //!< a_color1
        Color2,    //!< a_color2
        Color3,    //!< a_color3
        Indices,   //!< a_indices
        Weight,    //!< a_weight
        TexCoord0, //!< a_texcoord0
        TexCoord1, //!< a_texcoord1
        TexCoord2, //!< a_texcoord2
        TexCoord3, //!< a_texcoord3
        TexCoord4, //!< a_texcoord4
        TexCoord5, //!< a_texcoord5
        TexCoord6, //!< a_texcoord6
        TexCoord7, //!< a_texcoord7

        Count
    };
};

/// Vertex attribute type enum.
///
/// @attention C99's equivalent binding is `bgfx_attrib_type_t`.
///
struct AttribType
{
    /// Attribute types:
    enum Enum
    {
        Uint8,  //!< Uint8
        Uint10, //!< Uint10, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_UINT10`.
        Int16,  //!< Int16
        Half,   //!< Half, availability depends on: `BGFX_CAPS_VERTEX_ATTRIB_HALF`.
        Float,  //!< Float

        Count
    };
};

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
BGFX_HANDLE(PSOHandle)

struct VertexLayout
{
    VertexLayout();

    VertexLayout &begin(RendererType::Enum _renderer = RendererType::Noop);

    void end();

    VertexLayout &add(Attrib::Enum _attrib, uint8_t _num, AttribType::Enum _type, bool _normalized = false,
                      bool _asInt = false);

    VertexLayout &skip(uint8_t _num);

    void decode(Attrib::Enum _attrib, uint8_t &_num, AttribType::Enum &_type, bool &_normalized, bool &_asInt) const;

    bool has(Attrib::Enum _attrib) const
    {
        return UINT16_MAX != m_attributes[_attrib];
    }

    uint16_t getOffset(Attrib::Enum _attrib) const
    {
        return m_offset[_attrib];
    }

    uint16_t getStride() const
    {
        return m_stride;
    }

    uint32_t getSize(uint32_t _num) const
    {
        return _num * m_stride;
    }

    uint32_t m_hash;                      //!< Hash.
    uint16_t m_stride;                    //!< Stride.
    uint16_t m_offset[Attrib::Count];     //!< Attribute offsets.
    uint16_t m_attributes[Attrib::Count]; //!< Used attributes.
};

struct InitParams
{
    int width;
    int height;
    int samples;
    int maxDepth;
    void *hwnd;
};

enum ShaderType
{
    ShaderType_Vertex,
    ShaderType_Fragment,
};

	struct ViewMode
	{
		/// View modes:
		enum Enum
		{
			Default,         //!< Default sort order.
			Sequential,      //!< Sort in the same order in which submit calls were called.
			DepthAscending,  //!< Sort draw call depth in ascending order.
			DepthDescending, //!< Sort draw call depth in descending order.

			Count
		};
	};

/// View id.
typedef uint16_t ViewId;

void init(const InitParams &params);

VertexBufferHandle createVertexBuffer(const void *_data, uint32_t _size, const VertexLayout &_layout,
                                      uint16_t _flags = BGFX_BUFFER_NONE);

IndexBufferHandle createIndexBuffer(const void *_data, uint32_t _size, uint16_t _flags = BGFX_BUFFER_NONE);

ShaderHandle createShader(const void *_data, uint32_t _size, ShaderType _type);

ProgramHandle createProgram(ShaderHandle _vsh, ShaderHandle _fsh);

PSOHandle createPSO(ProgramHandle _program, const VertexLayout &_layout, uint16_t _flags);

void setViewTransform(ViewId _id, const void *_view, const void *_proj);

void setViewRect(ViewId _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);

void beginFrame(ViewId _id);

void endFrame();

void drawMesh(VertexBufferHandle _vbh, IndexBufferHandle _ibh, ProgramHandle _program, PSOHandle _pso, uint16_t _state, const void* _mtx);

} // namespace TinyRender
