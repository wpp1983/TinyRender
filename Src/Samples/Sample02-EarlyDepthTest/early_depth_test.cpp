#include <bx/math.h>
#include <bx/timer.h>

#include "tiny_render.h"

#include <windows.h>

// 常量
const int FrameCount = 2;
const int Width = 800;
const int Height = 600;

// 全局变量
HWND g_hwnd;

uint64_t g_timeOffset;

// 顶点着色器
const char *g_vertexShader = R"(
struct VS_INPUT
{
float3 pos : POSITION;
float4 color : COLOR;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = float4(input.pos, 1.0f);
    output.color = input.color;
    return output;
}

)";

// 像素着色器
const char *g_pixelShader = R"(
struct PS_INPUT
{
float4 pos : SV_POSITION;
float4 color : COLOR;
};

float4 main(PS_INPUT input) : SV_Target
{
    return input.color;
}

)";

// 窗口过程
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}



struct PosColorVertex
{    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout.begin()
            .add(TinyRender::Attrib::Position, 3, TinyRender::AttribType::Float)
            .add(TinyRender::Attrib::Color0, 4, TinyRender::AttribType::Uint8, true)
            .end();
    };

    static TinyRender::VertexLayout ms_layout;
};

TinyRender::VertexLayout PosColorVertex::ms_layout;

float TriangleSize = 0.5f;
static PosColorVertex s_TriangleVertices[] = {
    {0.0f, TriangleSize, 0.0f, 0xff000000}, 
    {TriangleSize, -TriangleSize, 0.0f, 0xff0000ff}, 
    {-TriangleSize, -TriangleSize, 0.0f, 0xff00ff00},
};

static PosColorVertex s_TriangleVertices2[] = {
    {-TriangleSize, TriangleSize, 0.0f, 0xff000000}, 
    {TriangleSize, TriangleSize, 0.0f, 0xff0000ff}, 
    {0, -TriangleSize, 0.0f, 0xff00ff00},
};

static const uint16_t s_TriangleTriList[] = {
    0, 1, 2,          // 0
};

 TinyRender::VertexBufferHandle vbh;
 TinyRender::VertexBufferHandle vbh2;
 TinyRender::IndexBufferHandle ibh;
 TinyRender::ProgramHandle program;
 TinyRender::PSOHandle pso;

void Init()
{
    TinyRender::InitParams initParams = {Width, Height, 1, 1, g_hwnd};
    TinyRender::init(initParams);

    // Create vertex stream declaration.
    PosColorVertex::init();

    // Create static vertex buffer.
    vbh = TinyRender::createVertexBuffer(
        // Static data can be passed with bgfx::makeRef
        s_TriangleVertices, sizeof(s_TriangleVertices), PosColorVertex::ms_layout);

    vbh2 = TinyRender::createVertexBuffer(
        // Static data can be passed with bgfx::makeRef
        s_TriangleVertices2, sizeof(s_TriangleVertices2), PosColorVertex::ms_layout);

    // Create static index buffer for triangle list rendering.
    ibh = TinyRender::createIndexBuffer(s_TriangleTriList, sizeof(s_TriangleTriList));

    TinyRender::ShaderHandle vsh =
        TinyRender::createShader(g_vertexShader, strlen(g_vertexShader), TinyRender::ShaderType::ShaderType_Vertex);
    TinyRender::ShaderHandle fsh =
        TinyRender::createShader(g_pixelShader, strlen(g_pixelShader), TinyRender::ShaderType::ShaderType_Fragment);

    program = TinyRender::createProgram(vsh, fsh);

    pso = TinyRender::createPSO(program, PosColorVertex::ms_layout, 0);
}

// 渲染
void Render()
{

    float time = (float)((bx::getHPCounter() - g_timeOffset) / double(bx::getHPFrequency()));

    const bx::Vec3 at = {0.0f, 0.0f, 0.0f};
    const bx::Vec3 eye = {0.0f, 0.0f, -35.0f};

    // Set view and projection matrix for view 0.
    {
        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(Width) / float(Height), 0.1f, 100.0f, true);
        TinyRender::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        TinyRender::setViewRect(0, 0, 0, uint16_t(Width), uint16_t(Height));
    }

    TinyRender::beginFrame(0);

    TinyRender::drawMesh(vbh, ibh, program, pso, 0, nullptr);
    TinyRender::drawMesh(vbh2, ibh, program, pso, 0, nullptr);

    TinyRender::endFrame();
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(0, CLASS_NAME, L"Direct3D 12 Triangle", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                             Width, Height, NULL, NULL, hInstance, NULL);

    if (g_hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(g_hwnd, nCmdShow);

    Init();

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        Render();
    }

    return 0;
}
