#pragma once

#define BX_CONFIG_DEBUG 1

#if BX_CONFIG_DEBUG
#	define BX_TRACE  _BGFX_TRACE
#	define BX_WARN   _BGFX_WARN
#	define BX_ASSERT _BGFX_ASSERT
#endif // BX_CONFIG_DEBUG

#include "TinyRender.h"
#include "Core/handlealloc.h"

namespace TinyRender
{
#if BX_COMPILER_CLANG_ANALYZER
	void __attribute__( (analyzer_noreturn) ) fatal(const char* _filePath, uint16_t _line, Fatal::Enum _code, const char* _format, ...);
#else
	void fatal(const char* _filePath, uint16_t _line, Fatal::Enum _code, const char* _format, ...);
#endif // BX_COMPILER_CLANG_ANALYZER

	void trace(const char* _filePath, uint16_t _line, const char* _format, ...);

	inline bool operator==(const VertexLayoutHandle& _lhs, const VertexLayoutHandle& _rhs) { return _lhs.idx == _rhs.idx; }
	inline bool operator==(const UniformHandle& _lhs,    const UniformHandle&    _rhs) { return _lhs.idx == _rhs.idx; }
}

#define _BGFX_TRACE(_format, ...)                                                       \
	BX_MACRO_BLOCK_BEGIN                                                                \
		TinyRender::trace(__FILE__, uint16_t(__LINE__), "BGFX " _format "\n", ##__VA_ARGS__); \
	BX_MACRO_BLOCK_END

#define _BGFX_WARN(_condition, _format, ...)          \
	BX_MACRO_BLOCK_BEGIN                              \
		if (!BX_IGNORE_C4127(_condition) )            \
		{                                             \
			BX_TRACE("WARN " _format, ##__VA_ARGS__); \
		}                                             \
	BX_MACRO_BLOCK_END

#define _BGFX_ASSERT(_condition, _format, ...)                                                                 \
	BX_MACRO_BLOCK_BEGIN                                                                                       \
		if (!BX_IGNORE_C4127(_condition)                                                                       \
		&&  bx::assertFunction(bx::Location::current(), "ASSERT " #_condition " -> " _format, ##__VA_ARGS__) ) \
		{                                                                                                      \
			TinyRender::fatal(__FILE__, uint16_t(__LINE__), TinyRender::Fatal::DebugCheck, _format, ##__VA_ARGS__);        \
		}                                                                                                      \
	BX_MACRO_BLOCK_END

#define BGFX_FATAL(_condition, _err, _format, ...)                             \
	BX_MACRO_BLOCK_BEGIN                                                       \
		if (!BX_IGNORE_C4127(_condition) )                                     \
		{                                                                      \
			TinyRender::fatal(__FILE__, uint16_t(__LINE__), _err, _format, ##__VA_ARGS__); \
		}                                                                      \
	BX_MACRO_BLOCK_END

#define BGFX_ERROR_CHECK(_condition, _err, _result, _msg, _format, ...) \
	if (!BX_IGNORE_C4127(_condition) )                                  \
	{                                                                   \
		BX_ERROR_SET(_err, _result, _msg);                              \
		BX_TRACE("%S: 0x%08x '%S' - " _format                           \
			, &bxErrorScope.getName()                                   \
			, _err->get().code                                          \
			, &_err->getMessage()                                       \
			, ##__VA_ARGS__                                             \
			);                                                          \
		return;                                                         \
	}

#define BGFX_CLEAR_COLOR_USE_PALETTE UINT16_C(0x8000)
#define BGFX_CLEAR_MASK (0                 \
			| BGFX_CLEAR_COLOR             \
			| BGFX_CLEAR_DEPTH             \
			| BGFX_CLEAR_STENCIL           \
			| BGFX_CLEAR_COLOR_USE_PALETTE \
			)

namespace TinyRender
{

	struct Clear
	{
		void set(uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil)
		{
			m_flags    = _flags;
			m_index[0] = uint8_t(_rgba>>24);
			m_index[1] = uint8_t(_rgba>>16);
			m_index[2] = uint8_t(_rgba>> 8);
			m_index[3] = uint8_t(_rgba>> 0);
			m_depth    = _depth;
			m_stencil  = _stencil;
		}

		void set(uint16_t _flags, float _depth, uint8_t _stencil, uint8_t _0, uint8_t _1, uint8_t _2, uint8_t _3, uint8_t _4, uint8_t _5, uint8_t _6, uint8_t _7)
		{
			m_flags = (_flags & ~BGFX_CLEAR_COLOR)
				| (0xff != (_0&_1&_2&_3&_4&_5&_6&_7) ? BGFX_CLEAR_COLOR|BGFX_CLEAR_COLOR_USE_PALETTE : 0)
				;
			m_index[0] = _0;
			m_index[1] = _1;
			m_index[2] = _2;
			m_index[3] = _3;
			m_index[4] = _4;
			m_index[5] = _5;
			m_index[6] = _6;
			m_index[7] = _7;
			m_depth    = _depth;
			m_stencil  = _stencil;
		}

		uint8_t  m_index[8];
		float    m_depth;
		uint8_t  m_stencil;
		uint16_t m_flags;
	};

	struct Rect
	{
		Rect()
		{
		}

		Rect(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
			: m_x(_x)
			, m_y(_y)
			, m_width(_width)
			, m_height(_height)
		{
		}

		void clear()
		{
			m_x      = 0;
			m_y      = 0;
			m_width  = 0;
			m_height = 0;
		}

		bool isZero() const
		{
			uint64_t ui64 = *( (uint64_t*)this);
			return UINT64_C(0) == ui64;
		}

		bool isZeroArea() const
		{
			return 0 == m_width
				|| 0 == m_height
				;
		}

		void set(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
		{
			m_x = _x;
			m_y = _y;
			m_width  = _width;
			m_height = _height;
		}



		uint16_t m_x;
		uint16_t m_y;
		uint16_t m_width;
		uint16_t m_height;
	};

	struct Matrix4
	{
		union
		{
			float val[16];
			// bx::float4x4_t f4x4;
		} un;

		void setIdentity()
		{
			Core::memSet(un.val, 0, sizeof(un.val) );
			un.val[0] = un.val[5] = un.val[10] = un.val[15] = 1.0f;
		}
	};

	struct View
	{
		void reset()
		{
			setRect(0, 0, 1, 1);
			setScissor(0, 0, 0, 0);
			setClear(BGFX_CLEAR_NONE, 0, 0.0f, 0);
			setMode(ViewMode::Default);
			setFrameBuffer(TinyRender_INVALID_HANDLE);
			setTransform(nullptr, nullptr);
		}

		void setRect(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
		{
			m_rect.m_x      = uint16_t(Core::max<int16_t>(int16_t(_x), 0) );
			m_rect.m_y      = uint16_t(Core::max<int16_t>(int16_t(_y), 0) );
			m_rect.m_width  = Core::max<uint16_t>(_width,  1);
			m_rect.m_height = Core::max<uint16_t>(_height, 1);
		}

		void setScissor(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
		{
			m_scissor.m_x = _x;
			m_scissor.m_y = _y;
			m_scissor.m_width  = _width;
			m_scissor.m_height = _height;
		}

		void setClear(uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil)
		{
			m_clear.set(_flags, _rgba, _depth, _stencil);
		}

		void setClear(uint16_t _flags, float _depth, uint8_t _stencil, uint8_t _0, uint8_t _1, uint8_t _2, uint8_t _3, uint8_t _4, uint8_t _5, uint8_t _6, uint8_t _7)
		{
			m_clear.set(_flags, _depth, _stencil, _0, _1, _2, _3, _4, _5, _6, _7);
		}

		void setMode(ViewMode::Enum _mode)
		{
			m_mode = uint8_t(_mode);
		}

		void setFrameBuffer(FrameBufferHandle _handle)
		{
			m_fbh = _handle;
		}

		void setTransform(const void* _view, const void* _proj)
		{
			if (nullptr != _view)
			{
				Core::memCopy(m_view.un.val, _view, sizeof(Matrix4) );
			}
			else
			{
				m_view.setIdentity();
			}

			if (nullptr != _proj)
			{
				Core::memCopy(m_proj.un.val, _proj, sizeof(Matrix4) );
			}
			else
			{
				m_proj.setIdentity();
			}
		}

		Clear   m_clear;
		Rect    m_rect;
		Rect    m_scissor;
		Matrix4 m_view;
		Matrix4 m_proj;
		FrameBufferHandle m_fbh;
		uint8_t m_mode;
	};



    struct Context
    {
        static Context* s_ctx;
        static constexpr uint32_t kAlignment = 64;

        bool Init(const InitParams& _init);

        InitParams m_init;

		View m_view[1];

        bool m_headless;
		bool m_rendererInitialized;
		bool m_exit;
		bool m_flipAfterRender;
		bool m_singleThreaded;
		bool m_flipped;


		void setViewClear(ViewId _id, uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil)
		{

			m_view[_id].setClear(_flags, _rgba, _depth, _stencil);
		}

		VertexBufferHandle createVertexBuffer(const Memory* _mem, const VertexLayout& _layout, uint16_t _flags)
		{
						
			VertexBufferHandle handle = { m_vertexBufferHandle.alloc() };

			if (isValid(handle) )
			{
				VertexLayoutHandle layoutHandle = findOrCreateVertexLayout(_layout);
				if (!isValid(layoutHandle) )
				{
					BX_TRACE("WARNING: Failed to allocate vertex layout handle (BGFX_CONFIG_MAX_VERTEX_LAYOUTS, max: %d).", BGFX_CONFIG_MAX_VERTEX_LAYOUTS);
					m_vertexBufferHandle.free(handle.idx);
					return BGFX_INVALID_HANDLE;
				}

				m_vertexLayoutRef.add(handle, layoutHandle, _layout.m_hash);

				VertexBuffer& vb = m_vertexBuffers[handle.idx];
				vb.m_size   = _mem->size;
				vb.m_stride = _layout.m_stride;

				CommandBuffer& cmdbuf = getCommandBuffer(CommandBuffer::CreateVertexBuffer);
				cmdbuf.write(handle);
				cmdbuf.write(_mem);
				cmdbuf.write(layoutHandle);
				cmdbuf.write(_flags);

				setDebugNameForHandle(handle);

				return handle;
			}

			BX_TRACE("WARNING: Failed to allocate vertex buffer handle (BGFX_CONFIG_MAX_VERTEX_BUFFERS, max: %d).", BGFX_CONFIG_MAX_VERTEX_BUFFERS);
			release(_mem);

			return BGFX_INVALID_HANDLE;

		}


		Core::HandleAllocT<BGFX_CONFIG_MAX_VERTEX_BUFFERS> m_vertexBufferHandle;
		Core::HandleAllocT<BGFX_CONFIG_MAX_SHADERS> m_shaderHandle;
		Core::HandleAllocT<BGFX_CONFIG_MAX_PROGRAMS> m_programHandle;
		Core::HandleAllocT<BGFX_CONFIG_MAX_TEXTURES> m_textureHandle;
		Core::HandleAllocT<BGFX_CONFIG_MAX_FRAME_BUFFERS> m_frameBufferHandle;
		Core::HandleAllocT<BGFX_CONFIG_MAX_UNIFORMS> m_uniformHandle;
		Core::HandleAllocT<BGFX_CONFIG_MAX_OCCLUSION_QUERIES> m_occlusionQueryHandle;
    };
}



