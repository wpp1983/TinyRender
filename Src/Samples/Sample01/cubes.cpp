#include "d3dx12.h"
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <windows.h>
#include <wrl/client.h>
#include <string.h>  // for strlen, memcpy
#include <stdlib.h>  // for _countof

#include "tiny_render.h"

using namespace DirectX;

// 常量
const UINT FrameCount = 2;
const UINT Width = 800;
const UINT Height = 600;

// 全局变量
HWND g_hwnd;
Microsoft::WRL::ComPtr<ID3D12Device> g_device;
Microsoft::WRL::ComPtr<ID3D12CommandQueue> g_commandQueue;
Microsoft::WRL::ComPtr<IDXGISwapChain3> g_swapChain;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> g_rtvHeap;
Microsoft::WRL::ComPtr<ID3D12Resource> g_renderTargets[FrameCount];
Microsoft::WRL::ComPtr<ID3D12CommandAllocator> g_commandAllocator;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> g_commandList;
Microsoft::WRL::ComPtr<ID3D12PipelineState> g_pipelineState;
Microsoft::WRL::ComPtr<ID3D12RootSignature> g_rootSignature;
Microsoft::WRL::ComPtr<ID3D12Fence> g_fence;
UINT64 g_fenceValue = 0;
HANDLE g_fenceEvent;
UINT g_rtvDescriptorSize;
UINT g_frameIndex;
Microsoft::WRL::ComPtr<ID3D12Resource> g_vertexBuffer;
D3D12_VERTEX_BUFFER_VIEW g_vertexBufferView;

// 顶点结构
struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

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

// 初始化D3D12
void InitD3D()
{
    // 创建设备
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapters1(0, &adapter);

    D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_device));

    // 创建命令队列
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    g_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_commandQueue));

    // 创建交换链
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = Width;
    swapChainDesc.Height = Height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
    factory->CreateSwapChainForHwnd(g_commandQueue.Get(), g_hwnd, &swapChainDesc, nullptr, nullptr, &swapChain);
    swapChain.As(&g_swapChain);
    g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();

    // 创建RTV描述符堆
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = FrameCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    g_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_rtvHeap));
    g_rtvDescriptorSize = g_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // 创建渲染目标视图
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < FrameCount; i++)
    {
        g_swapChain->GetBuffer(i, IID_PPV_ARGS(&g_renderTargets[i]));
        g_device->CreateRenderTargetView(g_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, g_rtvDescriptorSize);
    }

    // 创建命令分配器
    g_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_commandAllocator));

    // 创建根签名
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
    Microsoft::WRL::ComPtr<ID3DBlob> signature;
    Microsoft::WRL::ComPtr<ID3DBlob> error;
    D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    g_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                  IID_PPV_ARGS(&g_rootSignature));

    // 编译着色器
    Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
    D3DCompile(g_vertexShader, strlen(g_vertexShader), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShader,
               &error);
    D3DCompile(g_pixelShader, strlen(g_pixelShader), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShader,
               &error);

    // 定义顶点布局
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

    // 创建流水线状态
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
    psoDesc.pRootSignature = g_rootSignature.Get();
    psoDesc.VS = {vertexShader->GetBufferPointer(), vertexShader->GetBufferSize()};
    psoDesc.PS = {pixelShader->GetBufferPointer(), pixelShader->GetBufferSize()};
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    g_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&g_pipelineState));

    // 创建命令列表
    g_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_commandAllocator.Get(), g_pipelineState.Get(),
                                IID_PPV_ARGS(&g_commandList));
    g_commandList->Close();

    // 创建顶点缓冲区
    Vertex triangleVertices[] = {{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
                                 {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
                                 {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};

    const UINT vertexBufferSize = sizeof(triangleVertices);
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
    g_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                      nullptr, IID_PPV_ARGS(&g_vertexBuffer));

    // 复制顶点数据到顶点缓冲区
    UINT8 *pVertexDataBegin;
    CD3DX12_RANGE readRange(0, 0);
    g_vertexBuffer->Map(0, &readRange, reinterpret_cast<void **>(&pVertexDataBegin));
    memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
    g_vertexBuffer->Unmap(0, nullptr);

    // 初始化顶点缓冲区视图
    g_vertexBufferView.BufferLocation = g_vertexBuffer->GetGPUVirtualAddress();
    g_vertexBufferView.StrideInBytes = sizeof(Vertex);
    g_vertexBufferView.SizeInBytes = vertexBufferSize;

    // 创建 Fence 和 Event
    g_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence));
    g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

// 渲染
void Render()
{
    g_commandAllocator->Reset();
    g_commandList->Reset(g_commandAllocator.Get(), g_pipelineState.Get());

    D3D12_VIEWPORT viewport = {0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, 1.0f};
    D3D12_RECT scissorRect = {0, 0, static_cast<LONG>(Width), static_cast<LONG>(Height)};
    g_commandList->RSSetViewports(1, &viewport);
    g_commandList->RSSetScissorRects(1, &scissorRect);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        g_renderTargets[g_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    g_commandList->ResourceBarrier(1, &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_rtvHeap->GetCPUDescriptorHandleForHeapStart(), g_frameIndex,
                                            g_rtvDescriptorSize);
    g_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
    g_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    g_commandList->SetGraphicsRootSignature(g_rootSignature.Get());
    g_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_commandList->IASetVertexBuffers(0, 1, &g_vertexBufferView);

    g_commandList->DrawInstanced(3, 1, 0, 0);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(g_renderTargets[g_frameIndex].Get(),
                                                   D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    g_commandList->ResourceBarrier(1, &barrier);

    g_commandList->Close();

    ID3D12CommandList *ppCommandLists[] = {g_commandList.Get()};
    g_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    g_swapChain->Present(1, 0);

    g_commandQueue->Signal(g_fence.Get(), g_fenceValue);

    if (g_fence->GetCompletedValue() < g_fenceValue)
    {
        g_fence->SetEventOnCompletion(g_fenceValue, g_fenceEvent);
        WaitForSingleObject(g_fenceEvent, INFINITE);
    }

    g_fenceValue++;

    g_frameIndex = g_swapChain->GetCurrentBackBufferIndex();
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

    TinyRender::InitParams initParams = {Width, Height, 1, 1, g_hwnd};
    TinyRender::Init(initParams);


    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        Render();
    }

    return 0;
}
