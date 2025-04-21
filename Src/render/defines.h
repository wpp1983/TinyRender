#pragma once

#define BGFX_BUFFER_COMPUTE_FORMAT_8X1            UINT16_C(0x0001) //!< 1 8-bit value
#define BGFX_BUFFER_COMPUTE_FORMAT_8X2            UINT16_C(0x0002) //!< 2 8-bit values
#define BGFX_BUFFER_COMPUTE_FORMAT_8X4            UINT16_C(0x0003) //!< 4 8-bit values
#define BGFX_BUFFER_COMPUTE_FORMAT_16X1           UINT16_C(0x0004) //!< 1 16-bit value
#define BGFX_BUFFER_COMPUTE_FORMAT_16X2           UINT16_C(0x0005) //!< 2 16-bit values
#define BGFX_BUFFER_COMPUTE_FORMAT_16X4           UINT16_C(0x0006) //!< 4 16-bit values
#define BGFX_BUFFER_COMPUTE_FORMAT_32X1           UINT16_C(0x0007) //!< 1 32-bit value
#define BGFX_BUFFER_COMPUTE_FORMAT_32X2           UINT16_C(0x0008) //!< 2 32-bit values
#define BGFX_BUFFER_COMPUTE_FORMAT_32X4           UINT16_C(0x0009) //!< 4 32-bit values
#define BGFX_BUFFER_COMPUTE_FORMAT_SHIFT          0

#define BGFX_BUFFER_COMPUTE_FORMAT_MASK           UINT16_C(0x000f)

#define BGFX_BUFFER_COMPUTE_TYPE_INT              UINT16_C(0x0010) //!< Type `int`.
#define BGFX_BUFFER_COMPUTE_TYPE_UINT             UINT16_C(0x0020) //!< Type `uint`.
#define BGFX_BUFFER_COMPUTE_TYPE_FLOAT            UINT16_C(0x0030) //!< Type `float`.
#define BGFX_BUFFER_COMPUTE_TYPE_SHIFT            4

#define BGFX_BUFFER_COMPUTE_TYPE_MASK             UINT16_C(0x0030)

#define BGFX_BUFFER_NONE                          UINT16_C(0x0000)
#define BGFX_BUFFER_COMPUTE_READ                  UINT16_C(0x0100) //!< Buffer will be read by shader.
#define BGFX_BUFFER_COMPUTE_WRITE                 UINT16_C(0x0200) //!< Buffer will be used for writing.
#define BGFX_BUFFER_DRAW_INDIRECT                 UINT16_C(0x0400) //!< Buffer will be used for storing draw indirect commands.
#define BGFX_BUFFER_ALLOW_RESIZE                  UINT16_C(0x0800) //!< Allow dynamic index/vertex buffer resize during update.
#define BGFX_BUFFER_INDEX32                       UINT16_C(0x1000) //!< Index buffer contains 32-bit indices.
#define BGFX_BUFFER_COMPUTE_READ_WRITE (0 \
	| BGFX_BUFFER_COMPUTE_READ \
	| BGFX_BUFFER_COMPUTE_WRITE \
	)


#define BGFX_CLEAR_NONE                           UINT16_C(0x0000) //!< No clear flags.
#define BGFX_CLEAR_COLOR                          UINT16_C(0x0001) //!< Clear color.
#define BGFX_CLEAR_DEPTH                          UINT16_C(0x0002) //!< Clear depth.
#define BGFX_CLEAR_STENCIL                        UINT16_C(0x0004) //!< Clear stencil.

#define BGFX_DISCARD_NONE                         UINT8_C(0x00) //!< Preserve everything.
#define BGFX_DISCARD_BINDINGS                     UINT8_C(0x01) //!< Discard texture sampler and buffer bindings.
#define BGFX_DISCARD_INDEX_BUFFER                 UINT8_C(0x02) //!< Discard index buffer.
#define BGFX_DISCARD_INSTANCE_DATA                UINT8_C(0x04) //!< Discard instance data.
#define BGFX_DISCARD_STATE                        UINT8_C(0x08) //!< Discard state and uniform bindings.
#define BGFX_DISCARD_TRANSFORM                    UINT8_C(0x10) //!< Discard transform.
#define BGFX_DISCARD_VERTEX_STREAMS               UINT8_C(0x20) //!< Discard vertex streams.
#define BGFX_DISCARD_ALL                          UINT8_C(0xff) //!< Discard all states.

#ifndef BGFX_CONFIG_MAX_VERTEX_BUFFERS
#	define BGFX_CONFIG_MAX_VERTEX_BUFFERS (4<<10)
#endif // BGFX_CONFIG_MAX_VERTEX_BUFFERS

#ifndef BGFX_CONFIG_MAX_VERTEX_LAYOUTS
#	define BGFX_CONFIG_MAX_VERTEX_LAYOUTS 64
#endif // BGFX_CONFIG_MAX_VERTEX_LAYOUTS

#ifndef BGFX_CONFIG_MAX_INDEX_BUFFERS
#	define BGFX_CONFIG_MAX_INDEX_BUFFERS (4<<10)
#endif // BGFX_CONFIG_MAX_INDEX_BUFFERS

#ifndef BGFX_CONFIG_MAX_SHADERS
#	define BGFX_CONFIG_MAX_SHADERS 512
#endif // BGFX_CONFIG_MAX_FRAGMENT_SHADERS

#ifndef BGFX_CONFIG_MAX_PSOS
#	define BGFX_CONFIG_MAX_PSOS (4<<10)
#endif // BGFX_CONFIG_MAX_PSOS

#ifndef BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM
#	define BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM 9
#endif // BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM

// Cannot be configured via compiler options.
#define BGFX_CONFIG_MAX_PROGRAMS (1<<BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM)
// static_assert(bx::isPowerOf2(BGFX_CONFIG_MAX_PROGRAMS), "BGFX_CONFIG_MAX_PROGRAMS must be power of 2.");

#ifndef BGFX_CONFIG_MAX_VIEWS
#	define BGFX_CONFIG_MAX_VIEWS 256
#endif // BGFX_CONFIG_MAX_VIEWS


#ifndef BGFX_CONFIG_MAX_INSTANCE_DATA_COUNT
#	define BGFX_CONFIG_MAX_INSTANCE_DATA_COUNT 5
#endif // BGFX_CONFIG_MAX_INSTANCE_DATA_COUNT