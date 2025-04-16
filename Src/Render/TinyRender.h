#pragma once

#include "Core/core.h"
#include "defines.h"
#include <stdint.h>

#define TinyRender_HANDLE(_name)                                                           \
	struct _name { uint16_t idx; };                                                  \
	inline bool isValid(_name _handle) { return TinyRender::kInvalidHandle != _handle.idx; }

#define TinyRender_INVALID_HANDLE { TinyRender::kInvalidHandle }

namespace TinyRender
{
    	/// Fatal error enum.
	///
	/// @attention C99's equivalent binding is `bgfx_fatal_t`.
	///
	struct Fatal
	{
		enum Enum
		{
			DebugCheck,
			InvalidShader,
			UnableToInitialize,
			UnableToCreateTexture,
			DeviceLost,

			Count
		};
	};
    
	/// Renderer backend type enum.
	///
	/// @attention C99's equivalent binding is `bgfx_renderer_type_t`.
	///
	struct RendererType
	{
		/// Renderer types:
		enum Enum
		{
			Noop,         //!< No rendering.
			Agc,          //!< AGC
			Direct3D11,   //!< Direct3D 11.0
			Direct3D12,   //!< Direct3D 12.0
			Gnm,          //!< GNM
			Metal,        //!< Metal
			Nvn,          //!< NVN
			OpenGLES,     //!< OpenGL ES 2.0+
			OpenGL,       //!< OpenGL 2.1+
			Vulkan,       //!< Vulkan

			Count
		};
	};

	/// View mode sets draw call sort order.
	///
	/// @attention C99's equivalent binding is `bgfx_view_mode_t`.
	///
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

    	/// Texture format enum.
	///
	/// Notation:
	///
	///       RGBA16S
	///       ^   ^ ^
	///       |   | +-- [ ]Unorm
	///       |   |     [F]loat
	///       |   |     [S]norm
	///       |   |     [I]nt
	///       |   |     [U]int
	///       |   +---- Number of bits per component
	///       +-------- Components
	///
	/// @attention Availability depends on Caps (see: formats).
	///
	/// @attention C99's equivalent binding is `bgfx_texture_format_t`.
	///
	struct TextureFormat
	{
		/// Texture formats:
		enum Enum
		{
			BC1,          //!< DXT1 R5G6B5A1
			BC2,          //!< DXT3 R5G6B5A4
			BC3,          //!< DXT5 R5G6B5A8
			BC4,          //!< LATC1/ATI1 R8
			BC5,          //!< LATC2/ATI2 RG8
			BC6H,         //!< BC6H RGB16F
			BC7,          //!< BC7 RGB 4-7 bits per color channel, 0-8 bits alpha
			ETC1,         //!< ETC1 RGB8
			ETC2,         //!< ETC2 RGB8
			ETC2A,        //!< ETC2 RGBA8
			ETC2A1,       //!< ETC2 RGB8A1
			PTC12,        //!< PVRTC1 RGB 2BPP
			PTC14,        //!< PVRTC1 RGB 4BPP
			PTC12A,       //!< PVRTC1 RGBA 2BPP
			PTC14A,       //!< PVRTC1 RGBA 4BPP
			PTC22,        //!< PVRTC2 RGBA 2BPP
			PTC24,        //!< PVRTC2 RGBA 4BPP
			ATC,          //!< ATC RGB 4BPP
			ATCE,         //!< ATCE RGBA 8 BPP explicit alpha
			ATCI,         //!< ATCI RGBA 8 BPP interpolated alpha
			ASTC4x4,      //!< ASTC 4x4 8.0 BPP
			ASTC5x4,      //!< ASTC 5x4 6.40 BPP
			ASTC5x5,      //!< ASTC 5x5 5.12 BPP
			ASTC6x5,      //!< ASTC 6x5 4.27 BPP
			ASTC6x6,      //!< ASTC 6x6 3.56 BPP
			ASTC8x5,      //!< ASTC 8x5 3.20 BPP
			ASTC8x6,      //!< ASTC 8x6 2.67 BPP
			ASTC8x8,      //!< ASTC 8x8 2.00 BPP
			ASTC10x5,     //!< ASTC 10x5 2.56 BPP
			ASTC10x6,     //!< ASTC 10x6 2.13 BPP
			ASTC10x8,     //!< ASTC 10x8 1.60 BPP
			ASTC10x10,    //!< ASTC 10x10 1.28 BPP
			ASTC12x10,    //!< ASTC 12x10 1.07 BPP
			ASTC12x12,    //!< ASTC 12x12 0.89 BPP

			Unknown,      // Compressed formats above.

			R1,
			A8,
			R8,
			R8I,
			R8U,
			R8S,
			R16,
			R16I,
			R16U,
			R16F,
			R16S,
			R32I,
			R32U,
			R32F,
			RG8,
			RG8I,
			RG8U,
			RG8S,
			RG16,
			RG16I,
			RG16U,
			RG16F,
			RG16S,
			RG32I,
			RG32U,
			RG32F,
			RGB8,
			RGB8I,
			RGB8U,
			RGB8S,
			RGB9E5F,
			BGRA8,
			RGBA8,
			RGBA8I,
			RGBA8U,
			RGBA8S,
			RGBA16,
			RGBA16I,
			RGBA16U,
			RGBA16F,
			RGBA16S,
			RGBA32I,
			RGBA32U,
			RGBA32F,
			B5G6R5,
			R5G6B5,
			BGRA4,
			RGBA4,
			BGR5A1,
			RGB5A1,
			RGB10A2,
			RG11B10F,

			UnknownDepth, // Depth formats below.

			D16,
			D24,
			D24S8,
			D32,
			D16F,
			D24F,
			D32F,
			D0S8,

			Count
		};
	};

	/// Vertex layout.
	///
	/// @attention C99's equivalent binding is `bgfx_vertex_layout_t`.
	///
	struct VertexLayout
	{
		VertexLayout();

		/// Start VertexLayout.
		///
		/// @param[in] _renderer Renderer backend type. See: `bgfx::RendererType`
		/// @returns Returns itself.
		///
		/// @attention C99's equivalent binding is `bgfx_vertex_layout_begin`.
		///
		VertexLayout& begin(RendererType::Enum _renderer = RendererType::Noop);

		///
		/// @attention C99's equivalent binding is `bgfx_vertex_layout_end`.
		///
		/// End VertexLayout.
		void end();

		/// Add attribute to VertexLayout.
		///
		/// @param[in] _attrib Attribute semantics. See: `bgfx::Attrib`
		/// @param[in] _num Number of elements 1, 2, 3 or 4.
		/// @param[in] _type Element type.
		/// @param[in] _normalized When using fixed point AttribType (f.e. Uint8)
		///   value will be normalized for vertex shader usage. When normalized
		///   is set to true, AttribType::Uint8 value in range 0-255 will be
		///   in range 0.0-1.0 in vertex shader.
		/// @param[in] _asInt Packaging rule for vertexPack, vertexUnpack, and
		///   vertexConvert for AttribType::Uint8 and AttribType::Int16.
		///   Unpacking code must be implemented inside vertex shader.
		/// @returns Returns itself.
		///
		/// @remarks
		///   Must be called between begin/end.
		///
		/// @attention C99's equivalent binding is `bgfx_vertex_layout_add`.
		///
		VertexLayout& add(
			  Attrib::Enum _attrib
			, uint8_t _num
			, AttribType::Enum _type
			, bool _normalized = false
			, bool _asInt = false
			);

		/// Skip _num bytes in vertex stream.
		///
		/// @returns Returns itself.
		///
		/// @attention C99's equivalent binding is `bgfx_vertex_layout_skip`.
		///
		VertexLayout& skip(uint8_t _num);

		/// Decode attribute.
		///
		/// @attention C99's equivalent binding is `bgfx_vertex_layout_decode`.
		///
		void decode(
			  Attrib::Enum _attrib
			, uint8_t& _num
			, AttribType::Enum& _type
			, bool& _normalized
			, bool& _asInt
			) const;

		/// Returns `true` if VertexLayout contains attribute.
		///
		/// @param[in] _attrib Attribute semantics. See: `bgfx::Attrib`
		/// @returns True if VertexLayout contains attribute.
		///
		/// @attention C99's equivalent binding is `bgfx_vertex_layout_has`.
		///
		bool has(Attrib::Enum _attrib) const { return UINT16_MAX != m_attributes[_attrib]; }

		/// Returns relative attribute offset from the vertex.
		///
		/// @param[in] _attrib Attribute semantics. See: `bgfx::Attrib`
		/// @returns Relative attribute offset from the vertex.
		///
		uint16_t getOffset(Attrib::Enum _attrib) const { return m_offset[_attrib]; }

		/// Returns vertex stride.
		///
		/// @returns Vertex stride.
		///
		uint16_t getStride() const { return m_stride; }

		/// Returns size of vertex buffer for number of vertices.
		///
		/// @param[in] _num Number of vertices.
		/// @returns Size of vertex buffer for number of vertices.
		///
		uint32_t getSize(uint32_t _num) const { return _num*m_stride; }

		uint32_t m_hash;                      //!< Hash.
		uint16_t m_stride;                    //!< Stride.
		uint16_t m_offset[Attrib::Count];     //!< Attribute offsets.
		uint16_t m_attributes[Attrib::Count]; //!< Used attributes.
	};

	

    /// Native window handle type.
	///
	/// @attention C99's equivalent binding is `bgfx_native_window_handle_type_t`.
	///

	struct NativeWindowHandleType
	{
		 enum Enum
		 {
		       Default = 0, //!< Platform default handle type (X11 on Linux).
		       Wayland,     //!< Wayland.

		       Count
		 };
	};
    
    static const uint16_t kInvalidHandle = UINT16_MAX;

	TinyRender_HANDLE(DynamicIndexBufferHandle)
	TinyRender_HANDLE(DynamicVertexBufferHandle)
	TinyRender_HANDLE(FrameBufferHandle)
	TinyRender_HANDLE(IndexBufferHandle)
	TinyRender_HANDLE(IndirectBufferHandle)
	TinyRender_HANDLE(OcclusionQueryHandle)
	TinyRender_HANDLE(ProgramHandle)
	TinyRender_HANDLE(ShaderHandle)
	TinyRender_HANDLE(TextureHandle)
	TinyRender_HANDLE(UniformHandle)
	TinyRender_HANDLE(VertexBufferHandle)
	TinyRender_HANDLE(VertexLayoutHandle)

	/// Platform data.
	///
	/// @attention C99's equivalent binding is `bgfx_platform_data_t`.
	///
	struct PlatformData
	{
		PlatformData();

		void* ndt;                         //!< Native display type (*nix specific).
		void* nwh;                         //!< Native window handle. If `NULL`, bgfx will create a headless
		                                   ///  context/device, provided the rendering API supports it.
		void* context;                     //!< GL context, D3D device, or Vulkan device. If `NULL`, bgfx
		                                   ///  will create context/device.
		void* backBuffer;                  //!< GL back-buffer, or D3D render target view. If `NULL` bgfx will
		                                   ///  create back-buffer color surface.
		void* backBufferDS;                //!< Backbuffer depth/stencil. If `NULL`, bgfx will create a back-buffer
		                                   ///  depth/stencil surface.
		NativeWindowHandleType::Enum type; //!< Handle type. Needed for platforms having more than one option.
	};

    	/// Backbuffer resolution and reset parameters.
	///
	/// @attention C99's equivalent binding is `bgfx_resolution_t`.
	///
	struct Resolution
	{
		Resolution();

		TextureFormat::Enum format; //!< Backbuffer format.
		uint32_t width;             //!< Backbuffer width.
		uint32_t height;            //!< Backbuffer height.
		uint32_t reset;             //!< Reset parameters.
		uint8_t  numBackBuffers;    //!< Number of back buffers.
		uint8_t  maxFrameLatency;   //!< Maximum frame latency.
		uint8_t  debugTextScale;    //!< Scale factor for debug text.
	};

    /// Initialization parameters used by `bgfx::init`.
	///
	/// @attention C99's equivalent binding is `bgfx_init_t`.
	///
	struct InitParams
	{
		InitParams();

		/// Select rendering backend. When set to RendererType::Count
		/// a default rendering backend will be selected appropriate to the platform.
		/// See: `bgfx::RendererType`
		RendererType::Enum type;

		/// Vendor PCI ID. If set to `BGFX_PCI_ID_NONE`, discrete and integrated
		/// GPUs will be prioritised.
		///   - `BGFX_PCI_ID_NONE` - Auto-select adapter.
		///   - `BGFX_PCI_ID_SOFTWARE_RASTERIZER` - Software rasterizer.
		///   - `BGFX_PCI_ID_AMD` - AMD adapter.
		///   - `BGFX_PCI_ID_APPLE` - Apple adapter.
		///   - `BGFX_PCI_ID_INTEL` - Intel adapter.
		///   - `BGFX_PCI_ID_NVIDIA` - NVIDIA adapter.
		///   - `BGFX_PCI_ID_MICROSOFT` - Microsoft adapter.
		uint16_t vendorId;

		/// Device ID. If set to 0 it will select first device, or device with
		/// matching ID.
		uint16_t deviceId;

		uint64_t capabilities; //!< Capabilities initialization mask (default: UINT64_MAX).

		bool debug;   //!< Enable device for debugging.
		bool profile; //!< Enable device for profiling.

		/// Platform data.
		PlatformData platformData;

		/// Backbuffer resolution and reset parameters. See: `bgfx::Resolution`.
		Resolution resolution;

	};


    bool Init(InitParams const& _init);

	/// View id.
	typedef uint16_t ViewId;


	/// Memory must be obtained by calling `bgfx::alloc`, `bgfx::copy`, or `bgfx::makeRef`.
	///
	/// @attention It is illegal to create this structure on stack and pass it to any bgfx API.
	///
	/// @attention C99's equivalent binding is `bgfx_memory_t`.
	///
	struct Memory
	{
		Memory() = delete;

		uint8_t* data; //!< Pointer to data.
		uint32_t size; //!< Data size.
	};

	/// Set view clear flags.
	///
	/// @param[in] _id View id.
	/// @param[in] _flags Clear flags. Use `BGFX_CLEAR_NONE` to remove any clear
	///   operation. See: `BGFX_CLEAR_*`.
	/// @param[in] _rgba Color clear value.
	/// @param[in] _depth Depth clear value.
	/// @param[in] _stencil Stencil clear value.
	///
	/// @attention C99's equivalent binding is `bgfx_set_view_clear`.
	///
	void SetViewClear(
		  ViewId _id
		, uint16_t _flags
		, uint32_t _rgba = 0x000000ff
		, float _depth = 1.0f
		, uint8_t _stencil = 0
		);


	/// Create static vertex buffer.
	///
	/// @param[in] _mem Vertex buffer data.
	/// @param[in] _layout Vertex layout.
	/// @param[in] _flags Buffer creation flags.
	///   - `BGFX_BUFFER_NONE` - No flags.
	///   - `BGFX_BUFFER_COMPUTE_READ` - Buffer will be read from by compute shader.
	///   - `BGFX_BUFFER_COMPUTE_WRITE` - Buffer will be written into by compute shader. When buffer
	///       is created with `BGFX_BUFFER_COMPUTE_WRITE` flag it cannot be updated from CPU.
	///   - `BGFX_BUFFER_COMPUTE_READ_WRITE` - Buffer will be used for read/write by compute shader.
	///   - `BGFX_BUFFER_ALLOW_RESIZE` - Buffer will resize on buffer update if a different amount of
	///       data is passed. If this flag is not specified, and more data is passed on update, the buffer
	///       will be trimmed to fit the existing buffer size. This flag has effect only on dynamic
	///       buffers.
	///   - `BGFX_BUFFER_INDEX32` - Buffer is using 32-bit indices. This flag has effect only on
	///       index buffers.
	/// @returns Static vertex buffer handle.
	///
	/// @attention C99's equivalent binding is `bgfx_create_vertex_buffer`.
	///
	VertexBufferHandle createVertexBuffer(
		  const Memory* _mem
		, const VertexLayout& _layout
		, uint16_t _flags = BGFX_BUFFER_NONE
		);
}
