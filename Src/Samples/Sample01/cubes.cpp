#define ENTRY_CONFIG_IMPLEMENT_MAIN 1
#define ENTRY_CONFIG_USE_NATIVE 1
#define BX_PLATFORM_WINDOWS 1

#include "Core/core.h"
#include "Common/entry/entry.h"
#include "Render/TinyRender.h"
#include "Common/util.h"

namespace
{


struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

	static void init()
	{
		ms_layout
			.begin()
			.add(TinyRender::Attrib::Position, 3, TinyRender::AttribType::Float)
			.add(TinyRender::Attrib::Color0,   4, TinyRender::AttribType::Uint8, true)
			.end();
	};

	static TinyRender::VertexLayout ms_layout;
};

TinyRender::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeTriList[] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};

static const uint16_t s_cubeTriStrip[] =
{
	0, 1, 2,
	3,
	7,
	1,
	5,
	0,
	4,
	2,
	6,
	7,
	4,
	5,
};


static const uint16_t s_cubeLineList[] =
{
	0, 1,
	0, 2,
	0, 4,
	1, 3,
	1, 5,
	2, 3,
	2, 6,
	3, 7,
	4, 5,
	4, 6,
	5, 7,
	6, 7,
};

static const uint16_t s_cubeLineStrip[] =
{
	0, 2, 3, 1, 5, 7, 6, 4,
	0, 2, 6, 4, 5, 7, 3, 1,
	0,
};

static const uint16_t s_cubePoints[] =
{
	0, 1, 2, 3, 4, 5, 6, 7
};

static const char* s_ptNames[]
{
	"Triangle List",
	"Triangle Strip",
	"Lines",
	"Line Strip",
	"Points",
};

static const uint64_t s_ptState[]
{
	UINT64_C(0),
	BGFX_STATE_PT_TRISTRIP,
	BGFX_STATE_PT_LINES,
	BGFX_STATE_PT_LINESTRIP,
	BGFX_STATE_PT_POINTS,
};

class ExampleCubes : public Entry::AppInterface {
public:
    ExampleCubes( const char *_name, const char *_description, const char *_url) 
        : Entry::AppInterface(_name, _description, _url) {}


    void Initialize(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
	{
		Args args(_argc, _argv);

		m_width  = _width;
		m_height = _height;
		m_debug  = BGFX_DEBUG_NONE;
		m_reset  = BGFX_RESET_VSYNC;

		TinyRender::InitParams init;
		init.type     = args.m_type;
		init.vendorId = args.m_pciId;
		init.platformData.nwh  = Entry::getNativeWindowHandle(Entry::kDefaultWindowHandle);
		init.platformData.ndt  = Entry::getNativeDisplayHandle();
		init.platformData.type = TinyRender::NativeWindowHandleType::Default;
		init.resolution.width  = m_width;
		init.resolution.height = m_height;
		init.resolution.reset  = m_reset;
		TinyRender::Init(init);

		// Set view 0 clear state.
		TinyRender::SetViewClear(0
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
			);
	}

    void Update() override
	{

	}

    void Shutdown() override
	{

	}

private:
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;
	TinyRender::VertexBufferHandle m_vbh;
	TinyRender::IndexBufferHandle m_ibh[CORE_COUNTOF(s_ptState)];
	TinyRender::ProgramHandle m_program;
	int64_t m_timeOffset;
	int32_t m_pt;

	bool m_r;
	bool m_g;
	bool m_b;
	bool m_a;
};



}

ENTRY_IMPLEMENT_MAIN(
	  ExampleCubes
	, "Sample01"
	, "Initialization and debug text."
	, ""
	);

