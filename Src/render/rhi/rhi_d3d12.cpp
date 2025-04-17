#include "tiny_render_p.h"
#include "wingdi.h"

#if BGFX_CONFIG_RENDERER_DIRECT3D12

#include "rhi_d3d12.h"

#include "d3dx12.h"
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdlib.h> // for _countof
#include <string.h> // for strlen, memcpy
#include <windows.h>
#include <wrl/client.h>

namespace TinyRender
{
namespace d3d12
{
const UINT FrameCount = 2;

struct RendererContextD3D12 : public RendererContextI
{
    ~RendererContextD3D12() {}

    void Init(const InitParams &_init)
    {
        m_hwnd = (HWND)_init.hwnd;
        int Width = _init.width;
        int Height = _init.height;
        // 创建设备
        Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
        CreateDXGIFactory1(IID_PPV_ARGS(&factory));

        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        factory->EnumAdapters1(0, &adapter);

        D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

        // 创建命令队列
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

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
        factory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_hwnd, &swapChainDesc, nullptr, nullptr, &swapChain);
        swapChain.As(&m_swapChain);
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

        // 创建RTV描述符堆
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // 创建渲染目标视图
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < FrameCount; i++)
        {
            m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }

        // 创建命令分配器
        m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

        // 创建根签名
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;
        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
        m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                      IID_PPV_ARGS(&m_rootSignature));
    }

    void Shutdown() 
    {

    }

    void createIndexBuffer(IndexBufferHandle _handle, const void *_mem, uint16_t _flags) 
    {

    }

    void destroyIndexBuffer(IndexBufferHandle _handle) {}

    void createVertexLayout(VertexLayoutHandle _handle, const VertexLayout &_layout) {}

    void destroyVertexLayout(VertexLayoutHandle _handle) {}

    void createVertexBuffer(VertexBufferHandle _handle, const void *_mem, VertexLayoutHandle _layoutHandle,
                            uint16_t _flags)
    {

    }

    void destroyVertexBuffer(VertexBufferHandle _handle) {}

    HWND m_hwnd;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

    UINT m_frameIndex;
    UINT m_rtvDescriptorSize;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
};

static RendererContextD3D12 *s_renderD3D12;

RendererContextI *rendererCreate(const InitParams &_init)
{
    BX_ASSERT(s_renderD3D12 == nullptr, "Renderer already initialized");

    s_renderD3D12 = new RendererContextD3D12();
    s_renderD3D12->Init(_init);
    return s_renderD3D12;
}

void rendererDestroy()
{
    s_renderD3D12->Shutdown();
    delete s_renderD3D12;
    s_renderD3D12 = nullptr;
}


	void BufferD3D12::create(uint32_t _size, void* _data, uint16_t _flags, bool _vertex, uint32_t _stride)
	{
        m_size    = _size;
		m_flags   = _flags;

        DXGI_FORMAT format;
        uint32_t    stride;

        if (_vertex)
        {
            format = DXGI_FORMAT_R32G32B32_FLOAT;
            stride = _stride;
        }

        ID3D12Device* device = s_renderD3D12->m_device.Get();
		ID3D12GraphicsCommandList* commandList = s_renderD3D12->m_commandList.Get() ;
        

    	D3D12_RESOURCE_DESC resourceDesc = {};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width     = _size;
		resourceDesc.Height    = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format             = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count   = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags  = D3D12_RESOURCE_FLAG_NONE;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

        DX_CHECK(device->CreateCommittedResource(&heapProps, 
                        D3D12_HEAP_FLAG_NONE, 
                        &resourceDesc, 
                        D3D12_RESOURCE_STATE_GENERIC_READ,
                        nullptr, 
                        IID_PPV_ARGS(&m_ptr)));

        m_gpuVA = m_ptr->GetGPUVirtualAddress();

        if (_data)
        {
            
            update(commandList, 0, _size, _data, false);
        }
	}

	void BufferD3D12::update(ID3D12GraphicsCommandList* _commandList, uint32_t _offset, uint32_t _size, void* _data, bool _discard)
	{

	}

} // namespace d3d12
} // namespace TinyRender

#endif // BGFX_CONFIG_RENDERER_DIRECT3D12
