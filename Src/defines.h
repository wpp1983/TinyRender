#pragma once

#include <stdint.h>

#define BGFX_DEBUG_NONE                           UINT32_C(0x00000000) //!< No debug.
#define BGFX_DEBUG_WIREFRAME                      UINT32_C(0x00000001) //!< Enable wireframe for all primitives.

/// Enable infinitely fast hardware test. No draw calls will be submitted to driver.
/// It's useful when profiling to quickly assess bottleneck between CPU and GPU.
#define BGFX_DEBUG_IFH                            UINT32_C(0x00000002)
#define BGFX_DEBUG_STATS                          UINT32_C(0x00000004) //!< Enable statistics display.
#define BGFX_DEBUG_TEXT                           UINT32_C(0x00000008) //!< Enable debug text display.
#define BGFX_DEBUG_PROFILER                       UINT32_C(0x00000010) //!< Enable profiler. This causes per-view statistics to be collected, available through `bgfx::Stats::ViewStats`. This is unrelated to the profiler functions in `bgfx::CallbackI`.



#define BGFX_RESET_NONE                           UINT32_C(0x00000000) //!< No reset flags.
#define BGFX_RESET_FULLSCREEN                     UINT32_C(0x00000001) //!< Not supported yet.
#define BGFX_RESET_VSYNC                          UINT32_C(0x00000080) //!< Enable V-Sync.
#define BGFX_RESET_MAXANISOTROPY                  UINT32_C(0x00000100) //!< Turn on/off max anisotropy.
#define BGFX_RESET_CAPTURE                        UINT32_C(0x00000200) //!< Begin screen capture.
#define BGFX_RESET_FLUSH_AFTER_RENDER             UINT32_C(0x00002000) //!< Flush rendering after submitting to GPU.

#if !defined(ENTRY_DEFAULT_WIDTH) && !defined(ENTRY_DEFAULT_HEIGHT)
#	define ENTRY_DEFAULT_WIDTH  1280
#	define ENTRY_DEFAULT_HEIGHT 720
#elif !defined(ENTRY_DEFAULT_WIDTH) || !defined(ENTRY_DEFAULT_HEIGHT)
#	error "Both ENTRY_DEFAULT_WIDTH and ENTRY_DEFAULT_HEIGHT must be defined."
#endif // ENTRY_DEFAULT_WIDTH



#define BGFX_STATE_PT_TRISTRIP                    UINT64_C(0x0001000000000000) //!< Tristrip.
#define BGFX_STATE_PT_LINES                       UINT64_C(0x0002000000000000) //!< Lines.
#define BGFX_STATE_PT_LINESTRIP                   UINT64_C(0x0003000000000000) //!< Line strip.
#define BGFX_STATE_PT_POINTS                      UINT64_C(0x0004000000000000) //!< Points.
#define BGFX_STATE_PT_SHIFT                       48                           //!< Primitive type bit shift
#define BGFX_STATE_PT_MASK                        UINT64_C(0x0007000000000000) //!< Primitive type bit mask


#define BGFX_PCI_ID_NONE                          UINT16_C(0x0000) //!< Autoselect adapter.


#define BGFX_CLEAR_NONE                           UINT16_C(0x0000) //!< No clear flags.
#define BGFX_CLEAR_COLOR                          UINT16_C(0x0001) //!< Clear color.
#define BGFX_CLEAR_DEPTH                          UINT16_C(0x0002) //!< Clear depth.
#define BGFX_CLEAR_STENCIL                        UINT16_C(0x0004) //!< Clear stencil.
#define BGFX_CLEAR_DISCARD_COLOR_0                UINT16_C(0x0008) //!< Discard frame buffer attachment 0.
#define BGFX_CLEAR_DISCARD_COLOR_1                UINT16_C(0x0010) //!< Discard frame buffer attachment 1.
#define BGFX_CLEAR_DISCARD_COLOR_2                UINT16_C(0x0020) //!< Discard frame buffer attachment 2.
#define BGFX_CLEAR_DISCARD_COLOR_3                UINT16_C(0x0040) //!< Discard frame buffer attachment 3.
#define BGFX_CLEAR_DISCARD_COLOR_4                UINT16_C(0x0080) //!< Discard frame buffer attachment 4.
#define BGFX_CLEAR_DISCARD_COLOR_5                UINT16_C(0x0100) //!< Discard frame buffer attachment 5.
#define BGFX_CLEAR_DISCARD_COLOR_6                UINT16_C(0x0200) //!< Discard frame buffer attachment 6.
#define BGFX_CLEAR_DISCARD_COLOR_7                UINT16_C(0x0400) //!< Discard frame buffer attachment 7.
#define BGFX_CLEAR_DISCARD_DEPTH                  UINT16_C(0x0800) //!< Discard frame buffer depth attachment.
#define BGFX_CLEAR_DISCARD_STENCIL                UINT16_C(0x1000) //!< Discard frame buffer stencil attachment.
#define BGFX_CLEAR_DISCARD_COLOR_MASK (0 \
	| BGFX_CLEAR_DISCARD_COLOR_0 \
	| BGFX_CLEAR_DISCARD_COLOR_1 \
	| BGFX_CLEAR_DISCARD_COLOR_2 \
	| BGFX_CLEAR_DISCARD_COLOR_3 \
	| BGFX_CLEAR_DISCARD_COLOR_4 \
	| BGFX_CLEAR_DISCARD_COLOR_5 \
	| BGFX_CLEAR_DISCARD_COLOR_6 \
	| BGFX_CLEAR_DISCARD_COLOR_7 \
	)

#define BGFX_CLEAR_DISCARD_MASK (0 \
	| BGFX_CLEAR_DISCARD_COLOR_MASK \
	| BGFX_CLEAR_DISCARD_DEPTH \
	| BGFX_CLEAR_DISCARD_STENCIL \
	)


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



#ifndef BGFX_CONFIG_MAX_VERTEX_BUFFERS
#	define BGFX_CONFIG_MAX_VERTEX_BUFFERS (4<<10)
#endif // BGFX_CONFIG_MAX_VERTEX_BUFFERS

#ifndef BGFX_CONFIG_MAX_SHADERS
#	define BGFX_CONFIG_MAX_SHADERS 512
#endif // BGFX_CONFIG_MAX_SHADERS

#ifndef BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM
#	define BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM 9
#endif // BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM

// Cannot be configured via compiler options.
#define BGFX_CONFIG_MAX_PROGRAMS (1<<BGFX_CONFIG_SORT_KEY_NUM_BITS_PROGRAM)

#ifndef BGFX_CONFIG_MAX_TEXTURES
#	define BGFX_CONFIG_MAX_TEXTURES (4<<10)
#endif // BGFX_CONFIG_MAX_TEXTURES

#ifndef BGFX_CONFIG_MAX_FRAME_BUFFERS
#	define BGFX_CONFIG_MAX_FRAME_BUFFERS 128
#endif // BGFX_CONFIG_MAX_FRAME_BUFFERS

#ifndef BGFX_CONFIG_MAX_UNIFORMS
#	define BGFX_CONFIG_MAX_UNIFORMS 512
#endif // BGFX_CONFIG_MAX_UNIFORMS

#ifndef BGFX_CONFIG_MAX_OCCLUSION_QUERIES
#	define BGFX_CONFIG_MAX_OCCLUSION_QUERIES 256
#endif // BGFX_CONFIG_MAX_OCCLUSION_QUERIES










