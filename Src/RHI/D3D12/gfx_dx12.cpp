#include "RHI/gfx_api.hpp"
#include "ThirdParty/D3D12/directx/d3dx12.h"
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <vector>
#include "RHI/Vertex.hpp"
using namespace Microsoft::WRL;

class DX12Device : public GfxDevice {
    HWND hwnd = nullptr;
    ComPtr<ID3D12Device> device;
    ComPtr<IDXGISwapChain3> swapchain;
    ComPtr<ID3D12CommandQueue> cmdQueue;
    ComPtr<ID3D12CommandAllocator> allocator;
    ComPtr<ID3D12GraphicsCommandList> cmdList;
    ComPtr<ID3D12DescriptorHeap> rtvHeap;
    std::vector<ComPtr<ID3D12Resource>> backbuffers;
    UINT rtvDescriptorSize = 0;
    UINT frameIndex = 0;

    // 添加顶点缓冲区和着色器相关成员
    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12PipelineState> pipelineState;

public:
    bool initialize(void* hwnd_) override {
        hwnd = (HWND)hwnd_;

        // 1. DXGI + Device
        ComPtr<IDXGIFactory6> factory;
        CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
        ComPtr<IDXGIAdapter1> adapter;
        factory->EnumAdapters1(0, &adapter);
        D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

        // 2. Command queue
        D3D12_COMMAND_QUEUE_DESC qdesc = {};
        device->CreateCommandQueue(&qdesc, IID_PPV_ARGS(&cmdQueue));

        // 3. Swapchain
        DXGI_SWAP_CHAIN_DESC1 scDesc = {};
        scDesc.BufferCount = 2;
        scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.Width = 800;
        scDesc.Height = 600;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> tempSwap;
        factory->CreateSwapChainForHwnd(cmdQueue.Get(), hwnd, &scDesc, nullptr, nullptr, &tempSwap);
        tempSwap.As(&swapchain);
        frameIndex = swapchain->GetCurrentBackBufferIndex();

        // 4. RTV
        D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
        rtvDesc.NumDescriptors = 2;
        rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvHeap));
        rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
        backbuffers.resize(2);
        for (UINT i = 0; i < 2; ++i) {
            swapchain->GetBuffer(i, IID_PPV_ARGS(&backbuffers[i]));
            device->CreateRenderTargetView(backbuffers[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }

        // 5. Allocator + Command list
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
        device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
        cmdList->Close();

        // 6. 创建顶点缓冲区
        const UINT vertexBufferSize = sizeof(triangleVertices);
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&vertexBuffer));

        // 复制顶点数据到顶点缓冲区
        void* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        vertexBuffer->Map(0, &readRange, &pVertexDataBegin);
        memcpy(pVertexDataBegin, triangleVertices, vertexBufferSize);
        vertexBuffer->Unmap(0, nullptr);

        // 初始化顶点缓冲区视图
        vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
        vertexBufferView.StrideInBytes = sizeof(Vertex);
        vertexBufferView.SizeInBytes = vertexBufferSize;

        // 7. 创建根签名
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
        device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

        // 8. 编译着色器
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;
        ComPtr<ID3DBlob> errorBlob;

        const char* vsCode = R"(
            struct VSInput {
                float3 position : POSITION;
                float4 color : COLOR;
            };
            struct PSInput {
                float4 position : SV_POSITION;
                float4 color : COLOR;
            };
            PSInput VSMain(VSInput input) {
                PSInput output;
                output.position = float4(input.position, 1.0f);
                output.color = input.color;
                return output;
            }
        )";

        const char* psCode = R"(
            struct PSInput {
                float4 position : SV_POSITION;
                float4 color : COLOR;
            };
            float4 PSMain(PSInput input) : SV_TARGET {
                return input.color;
            }
        )";

        D3DCompile(vsCode, strlen(vsCode), nullptr, nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vertexShader, &errorBlob);
        D3DCompile(psCode, strlen(psCode), nullptr, nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &pixelShader, &errorBlob);

        // 9. 创建输入布局
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // 10. 创建流水线状态
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.pRootSignature = rootSignature.Get();
        psoDesc.VS = { vertexShader->GetBufferPointer(), vertexShader->GetBufferSize() };
        psoDesc.PS = { pixelShader->GetBufferPointer(), pixelShader->GetBufferSize() };
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

        return true;
    }

    void draw_frame() override {
        allocator->Reset();
        cmdList->Reset(allocator.Get(), pipelineState.Get());

        // 设置视口和裁剪矩形
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, 800.0f, 600.0f, 0.0f, 1.0f };
        D3D12_RECT scissorRect = { 0, 0, 800, 600 };
        cmdList->RSSetViewports(1, &viewport);
        cmdList->RSSetScissorRects(1, &scissorRect);

        // 设置渲染目标
        auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
        cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        // 清除渲染目标
        float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        // 设置根签名
        cmdList->SetGraphicsRootSignature(rootSignature.Get());

        // 设置顶点缓冲区
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);

        // 绘制三角形
        cmdList->DrawInstanced(3, 1, 0, 0);

        // 执行命令列表
        cmdList->Close();
        ID3D12CommandList* lists[] = { cmdList.Get() };
        cmdQueue->ExecuteCommandLists(1, lists);

        // 呈现
        swapchain->Present(1, 0);
        frameIndex = swapchain->GetCurrentBackBufferIndex();
    }

    void shutdown() override {
        cmdQueue->Signal(nullptr, 0); // 确保 GPU 完成
    }
};

GfxDevice* create_dx12_device() {
    return new DX12Device();
}