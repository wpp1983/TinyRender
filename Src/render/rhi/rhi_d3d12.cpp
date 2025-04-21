#include "tiny_render_p.h"

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

// Define the interface ID for ID3D12Resource
static const GUID IID_ID3D12Resource = {0x696442be, 0xa72e, 0x4059, {0xbc, 0x79, 0x5b, 0x5c, 0x98, 0x04, 0x0f, 0xad}};

struct RendererContextD3D12 : public RendererContextI
{
    ~RendererContextD3D12() {}

    void init(const InitParams &_init)
    {
        m_hwnd = static_cast<HWND>(_init.hwnd);
        int Width = _init.width;
        int Height = _init.height;

        // Enable debug layer
#if defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        debugController->EnableDebugLayer();
#endif
        
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

        // 创建命令列表
        m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

        // 创建根签名
        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
        Microsoft::WRL::ComPtr<ID3DBlob> signature;
        Microsoft::WRL::ComPtr<ID3DBlob> error;
        D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
        m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                      IID_PPV_ARGS(&m_rootSignature));

        // Create synchronization objects
        m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        m_fenceValue = 1;
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    }

    void Shutdown() {}

    void createIndexBuffer(IndexBufferHandle _handle, const void *_data, uint32_t _size, uint16_t _flags)
    {
        m_indexBuffers[_handle.idx].create(_size, _data, _flags, false);
    }

    void createVertexLayout(VertexLayoutHandle _handle, const VertexLayout &_layout)
    {
        VertexLayout &layout = m_vertexLayouts[_handle.idx];
        bx::memCopy(&layout, &_layout, sizeof(VertexLayout));
        dump(layout);
    }

    void destroyVertexLayout(VertexLayoutHandle _handle) {}

    void createVertexBuffer(VertexBufferHandle _handle, const void *_data, uint32_t _size,
                            VertexLayoutHandle _layoutHandle, uint16_t _flags)
    {
        m_vertexBuffers[_handle.idx].create(_size, _data, _layoutHandle, _flags);
    }

    void destroyVertexBuffer(VertexBufferHandle _handle) {}

    void destroyIndexBuffer(IndexBufferHandle _handle) {}

    void createShader(ShaderHandle _handle, const void *_data, uint32_t _size, ShaderType _type)
    {
        m_shaders[_handle.idx].create(_data, _size, _type);
    }

    void createProgram(ProgramHandle _handle, ShaderHandle _vsh, ShaderHandle _fsh)
    {
        m_program[_handle.idx].create(&m_shaders[_vsh.idx], &m_shaders[_fsh.idx]);
    }

    void createPSO(PSOHandle _handle, ProgramHandle _program, const VertexLayout &_layout, uint16_t _flags)
    {
        m_pso[_handle.idx].create(&m_program[_program.idx], &_layout, _flags);
    }

    void beginFrame(const View &_view)
    {
        m_commandAllocator->Reset();
        m_commandList->Reset(m_commandAllocator.Get(), nullptr);

        D3D12_VIEWPORT viewport = {static_cast<float>(_view.m_rect.m_x),
                                   static_cast<float>(_view.m_rect.m_y),
                                   static_cast<float>(_view.m_rect.m_width),
                                   static_cast<float>(_view.m_rect.m_height),
                                   0.0f,
                                   1.0f};
        D3D12_RECT scissorRect = {0, 0, static_cast<LONG>(_view.m_rect.m_width),
                                  static_cast<LONG>(_view.m_rect.m_height)};
        m_commandList->RSSetViewports(1, &viewport);
        m_commandList->RSSetScissorRects(1, &scissorRect);

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &barrier);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
                                                m_rtvDescriptorSize);
        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

        const float clearColor[] = {0.0f, 0.2f, 0.4f, 1.0f};
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        // m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    }

    void endFrame()
    {
        // Indicate that the back buffer will now be used to present.
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &barrier);
        
        m_commandList->Close();

        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
        m_swapChain->Present(1, 0);
        
        const UINT64 fence = m_fenceValue;
        m_commandQueue->Signal(m_fence.Get(), fence);
        m_fenceValue++;
        if (m_fence->GetCompletedValue() < fence)
        {
            m_fence->SetEventOnCompletion(fence, m_fenceEvent);
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    void drawMesh(VertexBufferHandle _vbh, IndexBufferHandle _ibh, ProgramHandle _program, PSOHandle _pso,
                  uint16_t _state, const void *_mtx)
    {
        // 设置顶点缓冲区
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        VertexLayoutHandle layoutHandle = m_vertexBuffers[_vbh.idx].m_layoutHandle;
        VertexLayout &layout = m_vertexLayouts[layoutHandle.idx];

        D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
        vertexBufferView.BufferLocation = m_vertexBuffers[_vbh.idx].m_gpuVA;
        vertexBufferView.SizeInBytes = m_vertexBuffers[_vbh.idx].m_size;
        vertexBufferView.StrideInBytes = layout.getStride();
        m_commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

        // 设置索引缓冲区
        D3D12_INDEX_BUFFER_VIEW indexBufferView;
        indexBufferView.BufferLocation = m_indexBuffers[_ibh.idx].m_gpuVA;
        indexBufferView.SizeInBytes = m_indexBuffers[_ibh.idx].m_size;
        indexBufferView.Format = m_indexBuffers[_ibh.idx].m_srvd.Format;
        m_commandList->IASetIndexBuffer(&indexBufferView);

        // 设置程序
        m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

        // 设置PSO
        m_commandList->SetPipelineState(m_pso[_pso.idx].m_pso);

        // 设置常量缓冲区
        // m_commandList->SetGraphicsRootConstantBufferView(0, m_constantBuffer.GetGPUVirtualAddress());

        // 绘制
        m_commandList->DrawIndexedInstanced(m_indexBuffers[_ibh.idx].m_srvd.Format == DXGI_FORMAT_R16_UINT ? 36 : 36, 1,
                                            0, 0, 0);


    }

    HWND m_hwnd;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

    // CommandQueueD3D12 m_cmd;
    UINT64 m_fenceValue;
    HANDLE m_fenceEvent;
    UINT m_frameIndex;
    UINT m_rtvDescriptorSize;

    VertexLayout m_vertexLayouts[BGFX_CONFIG_MAX_VERTEX_LAYOUTS];

    BufferD3D12 m_indexBuffers[BGFX_CONFIG_MAX_INDEX_BUFFERS];
    VertexBufferD3D12 m_vertexBuffers[BGFX_CONFIG_MAX_VERTEX_BUFFERS];

    ShaderD3D12 m_shaders[BGFX_CONFIG_MAX_SHADERS];
    ProgramD3D12 m_program[BGFX_CONFIG_MAX_PROGRAMS];
    PSOD3D12 m_pso[BGFX_CONFIG_MAX_PSOS];
};

static RendererContextD3D12 *s_renderD3D12;

RendererContextI *rendererCreate(const InitParams &_init)
{
    BX_ASSERT(s_renderD3D12 == nullptr, "Renderer already initialized");

    s_renderD3D12 = new RendererContextD3D12();
    s_renderD3D12->init(_init);
    return s_renderD3D12;
}

void rendererDestroy()
{
    s_renderD3D12->Shutdown();
    delete s_renderD3D12;
    s_renderD3D12 = nullptr;
}

void setResourceBarrier(ID3D12GraphicsCommandList *_commandList, const ID3D12Resource *_resource,
                        D3D12_RESOURCE_STATES _stateBefore, D3D12_RESOURCE_STATES _stateAfter)
{
    D3D12_RESOURCE_BARRIER barrier;
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = const_cast<ID3D12Resource *>(_resource);
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = _stateBefore;
    barrier.Transition.StateAfter = _stateAfter;
    _commandList->ResourceBarrier(1, &barrier);
}

struct HeapProperty
{
    enum Enum
    {
        Default,
        Texture,
        Upload,
        ReadBack,

        Count
    };

    D3D12_HEAP_PROPERTIES m_properties;
    D3D12_RESOURCE_STATES m_state;
};

static HeapProperty s_heapProperties[] = {
    {{D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0},
     D3D12_RESOURCE_STATE_COMMON},
    {{D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0},
     D3D12_RESOURCE_STATE_COMMON},
    {{D3D12_HEAP_TYPE_UPLOAD, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0},
     D3D12_RESOURCE_STATE_GENERIC_READ},
    {{D3D12_HEAP_TYPE_READBACK, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0},
     D3D12_RESOURCE_STATE_COPY_DEST},
};
static_assert(BX_COUNTOF(s_heapProperties) == HeapProperty::Count);

ID3D12Resource *createCommittedResource(ID3D12Device *_device, HeapProperty::Enum _heapProperty,
                                        const D3D12_RESOURCE_DESC *_resourceDesc, const D3D12_CLEAR_VALUE *_clearValue,
                                        bool _memSet = false)
{
    const HeapProperty &heapProperty = s_heapProperties[_heapProperty];
    ID3D12Resource *resource;
    DX_CHECK(_device->CreateCommittedResource(&heapProperty.m_properties, D3D12_HEAP_FLAG_NONE, _resourceDesc,
                                              heapProperty.m_state, _clearValue, IID_ID3D12Resource,
                                              (void **)&resource));
    BX_WARN(NULL != resource, "CreateCommittedResource failed (size: %d). Out of memory?", _resourceDesc->Width);

    if (BX_ENABLED(BX_PLATFORM_XBOXONE) && _memSet)
    {
        void *ptr;
        DX_CHECK(resource->Map(0, NULL, &ptr));
        D3D12_RESOURCE_ALLOCATION_INFO rai;
#if BX_COMPILER_MSVC || (BX_COMPILER_CLANG && defined(_MSC_VER))
        rai = _device->GetResourceAllocationInfo(1, 1, _resourceDesc);
#else
        _device->GetResourceAllocationInfo(&rai, 1, 1, _resourceDesc);
#endif // BX_COMPILER_MSVC || (BX_COMPILER_CLANG && defined(_MSC_VER))
        bx::memSet(ptr, 0, size_t(rai.SizeInBytes));
        resource->Unmap(0, NULL);
    }

    return resource;
}

ID3D12Resource *createCommittedResource(ID3D12Device *_device, HeapProperty::Enum _heapProperty, uint64_t _size,
                                        D3D12_RESOURCE_FLAGS _flags = D3D12_RESOURCE_FLAG_NONE)
{
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = _size;
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = _flags;

    return createCommittedResource(_device, _heapProperty, &resourceDesc, NULL);
}

struct UavFormat
{
    DXGI_FORMAT format[3];
    uint32_t stride;
};

static const UavFormat s_uavFormat[] = {
    //  BGFX_BUFFER_COMPUTE_TYPE_INT,  BGFX_BUFFER_COMPUTE_TYPE_UINT,  BGFX_BUFFER_COMPUTE_TYPE_FLOAT
    {{DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN, DXGI_FORMAT_UNKNOWN}, 0},             // ignored
    {{DXGI_FORMAT_R8_SINT, DXGI_FORMAT_R8_UINT, DXGI_FORMAT_UNKNOWN}, 1},             // BGFX_BUFFER_COMPUTE_FORMAT_8X1
    {{DXGI_FORMAT_R8G8_SINT, DXGI_FORMAT_R8G8_UINT, DXGI_FORMAT_UNKNOWN}, 2},         // BGFX_BUFFER_COMPUTE_FORMAT_8X2
    {{DXGI_FORMAT_R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_UINT, DXGI_FORMAT_UNKNOWN}, 4}, // BGFX_BUFFER_COMPUTE_FORMAT_8X4
    {{DXGI_FORMAT_R16_SINT, DXGI_FORMAT_R16_UINT, DXGI_FORMAT_R16_FLOAT}, 2},         // BGFX_BUFFER_COMPUTE_FORMAT_16X1
    {{DXGI_FORMAT_R16G16_SINT, DXGI_FORMAT_R16G16_UINT, DXGI_FORMAT_R16G16_FLOAT},
     4}, // BGFX_BUFFER_COMPUTE_FORMAT_16X2
    {{DXGI_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_FLOAT},
     8},                                                                      // BGFX_BUFFER_COMPUTE_FORMAT_16X4
    {{DXGI_FORMAT_R32_SINT, DXGI_FORMAT_R32_UINT, DXGI_FORMAT_R32_FLOAT}, 4}, // BGFX_BUFFER_COMPUTE_FORMAT_32X1
    {{DXGI_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32_FLOAT},
     8}, // BGFX_BUFFER_COMPUTE_FORMAT_32X2
    {{DXGI_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_FLOAT},
     16}, // BGFX_BUFFER_COMPUTE_FORMAT_32X4
};

void BufferD3D12::create(uint32_t _size, const void *_data, uint16_t _flags, bool _vertex, uint32_t _stride)
{
    m_size = _size;
    m_flags = _flags;

    const bool needUav = 0 != (_flags & (BGFX_BUFFER_COMPUTE_WRITE | BGFX_BUFFER_DRAW_INDIRECT));
    const bool drawIndirect = 0 != (_flags & BGFX_BUFFER_DRAW_INDIRECT);
    m_dynamic = NULL == _data || needUav;

    DXGI_FORMAT format;
    uint32_t stride;

    uint32_t flags = needUav ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS : D3D12_RESOURCE_FLAG_NONE;

    if (drawIndirect)
    {
        format = _vertex ? DXGI_FORMAT_R32G32B32A32_UINT : DXGI_FORMAT_R32_UINT;
        stride = _vertex ? 16 : 4;
    }
    else
    {
        uint32_t uavFormat = (_flags & BGFX_BUFFER_COMPUTE_FORMAT_MASK) >> BGFX_BUFFER_COMPUTE_FORMAT_SHIFT;
        if (0 == uavFormat)
        {
            if (_vertex)
            {
                format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                stride = 16;
            }
            else
            {
                if (0 == (_flags & BGFX_BUFFER_INDEX32))
                {
                    format = DXGI_FORMAT_R16_UINT;
                    stride = 2;
                }
                else
                {
                    format = DXGI_FORMAT_R32_UINT;
                    stride = 4;
                }
            }
        }
        else
        {
            const uint32_t uavType =
                bx::uint32_satsub((_flags & BGFX_BUFFER_COMPUTE_TYPE_MASK) >> BGFX_BUFFER_COMPUTE_TYPE_SHIFT, 1);
            format = s_uavFormat[uavFormat].format[uavType];
            stride = s_uavFormat[uavFormat].stride;
        }
    }

    stride = 0 == _stride ? stride : _stride;

    m_srvd.Format = format;
    m_srvd.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    m_srvd.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    m_srvd.Buffer.FirstElement = 0;
    m_srvd.Buffer.NumElements = m_size / stride;
    m_srvd.Buffer.StructureByteStride = 0;
    m_srvd.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

    m_uavd.Format = format;
    m_uavd.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
    m_uavd.Buffer.FirstElement = 0;
    m_uavd.Buffer.NumElements = m_size / stride;
    m_uavd.Buffer.StructureByteStride = 0;
    m_uavd.Buffer.CounterOffsetInBytes = 0;
    m_uavd.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

    ID3D12Device *device = s_renderD3D12->m_device.Get();
    ID3D12GraphicsCommandList *commandList = s_renderD3D12->m_commandList.Get();

    m_ptr = createCommittedResource(device, HeapProperty::Default, _size, D3D12_RESOURCE_FLAGS(flags));
    m_gpuVA = m_ptr->GetGPUVirtualAddress();
    setState(commandList, drawIndirect ? D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT : D3D12_RESOURCE_STATE_GENERIC_READ);

    if (!m_dynamic)
    {
        update(commandList, 0, _size, _data);
    }
}

void BufferD3D12::update(ID3D12GraphicsCommandList *_commandList, uint32_t _offset, uint32_t _size, const void *_data,
                         bool _discard)
{
    ID3D12Resource *staging = createCommittedResource(s_renderD3D12->m_device.Get(), HeapProperty::Upload, _size);
    uint8_t *data;

    D3D12_RANGE readRange = {0, 0};
    DX_CHECK(staging->Map(0, &readRange, (void **)&data));
    bx::memCopy(data, _data, _size);
    D3D12_RANGE writeRange = {0, _size};
    staging->Unmap(0, &writeRange);

    D3D12_RESOURCE_STATES state = setState(_commandList, D3D12_RESOURCE_STATE_COPY_DEST);
    _commandList->CopyBufferRegion(m_ptr, _offset, staging, 0, _size);
    setState(_commandList, state);

    // TODO: 释放资源
    //  s_renderD3D12->m_cmd.release(staging);
}

void BufferD3D12::destroy()
{
    if (m_ptr)
    {
        // TODO: 释放资源
        //  s_renderD3D12->m_cmd.release(m_ptr);
        m_ptr->Release();
        m_ptr = nullptr;
        m_dynamic = false;
        m_state = D3D12_RESOURCE_STATE_COMMON;
    }
}

D3D12_RESOURCE_STATES BufferD3D12::setState(ID3D12GraphicsCommandList *_commandList, D3D12_RESOURCE_STATES _state)
{
    if (m_state != _state)
    {
        setResourceBarrier(_commandList, m_ptr, m_state, _state);

        bx::swap(m_state, _state);
    }

    return _state;
}

void VertexBufferD3D12::create(uint32_t _size, const void *_data, VertexLayoutHandle _layoutHandle, uint16_t _flags)
{
    m_layoutHandle = _layoutHandle;
    BufferD3D12::create(_size, _data, _flags, true, 0);
}

void ShaderD3D12::create(const void *_data, uint32_t _size, ShaderType _type)
{
    BX_ASSERT(NULL != _data, "Invalid memory.");

    m_code = _data;
    m_size = _size;
    m_type = _type;

    // compile shader
    Microsoft::WRL::ComPtr<ID3DBlob> error;

    const char *entry = _type == ShaderType::ShaderType_Vertex ? "main" : "main";
    const char *target = _type == ShaderType::ShaderType_Vertex ? "vs_5_0" : "ps_5_0";

    DX_CHECK(D3DCompile(_data, _size, nullptr, nullptr, nullptr, entry, target, 0, 0, &m_shader, &error));

    if (error)
    {
        OutputDebugStringA(reinterpret_cast<const char *>(error->GetBufferPointer()));
    }
}

void ProgramD3D12::create(const ShaderD3D12 *_vsh, const ShaderD3D12 *_fsh)
{
    BX_ASSERT(NULL != _vsh->m_code, "Vertex shader doesn't exist.");
    m_vsh = _vsh;

    if (NULL != _fsh)
    {
        BX_ASSERT(NULL != _fsh->m_code, "Fragment shader doesn't exist.");
        m_fsh = _fsh;
    }
}

	static const D3D12_INPUT_ELEMENT_DESC s_attrib[] =
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BITANGENT",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",        0, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",        1, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",        2, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR",        3, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT,   0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     1, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     2, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     3, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     4, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     5, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     6, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",     7, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	static_assert(Attrib::Count == BX_COUNTOF(s_attrib) );

    	static const DXGI_FORMAT s_attribType[][4][2] =
	{
		{ // Uint8
			{ DXGI_FORMAT_R8_UINT,            DXGI_FORMAT_R8_UNORM           },
			{ DXGI_FORMAT_R8G8_UINT,          DXGI_FORMAT_R8G8_UNORM         },
			{ DXGI_FORMAT_R8G8B8A8_UINT,      DXGI_FORMAT_R8G8B8A8_UNORM     },
			{ DXGI_FORMAT_R8G8B8A8_UINT,      DXGI_FORMAT_R8G8B8A8_UNORM     },
		},
		{ // Uint10
			{ DXGI_FORMAT_R10G10B10A2_UINT,   DXGI_FORMAT_R10G10B10A2_UNORM  },
			{ DXGI_FORMAT_R10G10B10A2_UINT,   DXGI_FORMAT_R10G10B10A2_UNORM  },
			{ DXGI_FORMAT_R10G10B10A2_UINT,   DXGI_FORMAT_R10G10B10A2_UNORM  },
			{ DXGI_FORMAT_R10G10B10A2_UINT,   DXGI_FORMAT_R10G10B10A2_UNORM  },
		},
		{ // Int16
			{ DXGI_FORMAT_R16_SINT,           DXGI_FORMAT_R16_SNORM          },
			{ DXGI_FORMAT_R16G16_SINT,        DXGI_FORMAT_R16G16_SNORM       },
			{ DXGI_FORMAT_R16G16B16A16_SINT,  DXGI_FORMAT_R16G16B16A16_SNORM },
			{ DXGI_FORMAT_R16G16B16A16_SINT,  DXGI_FORMAT_R16G16B16A16_SNORM },
		},
		{ // Half
			{ DXGI_FORMAT_R16_FLOAT,          DXGI_FORMAT_R16_FLOAT          },
			{ DXGI_FORMAT_R16G16_FLOAT,       DXGI_FORMAT_R16G16_FLOAT       },
			{ DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT },
			{ DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT },
		},
		{ // Float
			{ DXGI_FORMAT_R32_FLOAT,          DXGI_FORMAT_R32_FLOAT          },
			{ DXGI_FORMAT_R32G32_FLOAT,       DXGI_FORMAT_R32G32_FLOAT       },
			{ DXGI_FORMAT_R32G32B32_FLOAT,    DXGI_FORMAT_R32G32B32_FLOAT    },
			{ DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT },
		},
	};
	static_assert(AttribType::Count == BX_COUNTOF(s_attribType) );

	static D3D12_INPUT_ELEMENT_DESC* fillVertexLayout(uint8_t _stream, D3D12_INPUT_ELEMENT_DESC* _out, const VertexLayout& _layout)
	{
		D3D12_INPUT_ELEMENT_DESC* elem = _out;

		for (uint32_t attr = 0; attr < Attrib::Count; ++attr)
		{
			if (UINT16_MAX != _layout.m_attributes[attr])
			{
				bx::memCopy(elem, &s_attrib[attr], sizeof(D3D12_INPUT_ELEMENT_DESC) );

				elem->InputSlot = _stream;

				if (0 == _layout.m_attributes[attr])
				{
					elem->AlignedByteOffset = 0;
				}
				else
				{
					uint8_t num;
					AttribType::Enum type;
					bool normalized;
					bool asInt;
					_layout.decode(Attrib::Enum(attr), num, type, normalized, asInt);
					elem->Format = s_attribType[type][num-1][normalized];
					elem->AlignedByteOffset = _layout.m_offset[attr];
				}

				++elem;
			}
		}

		return elem;
	}


uint32_t setInputLayout(D3D12_INPUT_ELEMENT_DESC *_vertexElements, uint8_t _numStreams, const VertexLayout **_layouts,
                        const ProgramD3D12 &_program, uint16_t _numInstanceData)
{
    // uint16_t attrMask[Attrib::Count];
    // bx::memCopy(attrMask, _program.m_vsh->m_attrMask, sizeof(attrMask));

    D3D12_INPUT_ELEMENT_DESC *elem = _vertexElements;

    for (uint8_t stream = 0; stream < _numStreams; ++stream)
    {
        VertexLayout layout;
        bx::memCopy(&layout, _layouts[stream], sizeof(VertexLayout));

        // const bool last = stream == _numStreams - 1;

        // for (uint32_t ii = 0; ii < Attrib::Count; ++ii)
        // {
        //     uint16_t mask = attrMask[ii];
        //     uint16_t attr = (layout.m_attributes[ii] & mask);
        //     if (0 == attr || UINT16_MAX == attr)
        //     {
        //         layout.m_attributes[ii] = last ? ~attr : UINT16_MAX;
        //     }
        //     else
        //     {
        //         attrMask[ii] = 0;
        //     }
        // }

        elem = fillVertexLayout(stream, elem, layout);
    }

    uint32_t num = uint32_t(elem - _vertexElements);

    const D3D12_INPUT_ELEMENT_DESC inst = {"TEXCOORD",
                                           0,
                                           DXGI_FORMAT_R32G32B32A32_FLOAT,
                                           0,
                                           D3D12_APPEND_ALIGNED_ELEMENT,
                                           D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA,
                                           1};

    for (uint32_t ii = 0; ii < _numInstanceData; ++ii)
    {
        uint32_t index = 7 - ii; // TEXCOORD7 = i_data0, TEXCOORD6 = i_data1, etc.

        uint32_t jj;
        D3D12_INPUT_ELEMENT_DESC *curr = _vertexElements;
        for (jj = 0; jj < num; ++jj)
        {
            curr = &_vertexElements[jj];
            if (0 == bx::strCmp(curr->SemanticName, "TEXCOORD") && curr->SemanticIndex == index)
            {
                break;
            }
        }

        if (jj == num)
        {
            curr = elem;
            ++elem;
        }

        bx::memCopy(curr, &inst, sizeof(D3D12_INPUT_ELEMENT_DESC));
        curr->InputSlot = _numStreams;
        curr->SemanticIndex = index;
        curr->AlignedByteOffset = ii * 16;
    }

    return uint32_t(elem - _vertexElements);
}

uint32_t setInputLayout(D3D12_INPUT_ELEMENT_DESC *_vertexElements, const VertexLayout &_layout,
                        const ProgramD3D12 &_program, uint16_t _numInstanceData)
{
    const VertexLayout *layouts[1] = {&_layout};
    return setInputLayout(_vertexElements, BX_COUNTOF(layouts), layouts, _program, _numInstanceData);
}

void PSOD3D12::create(const ProgramD3D12 *_program, const VertexLayout *_layout, uint16_t _flags)
{
    BX_ASSERT(NULL != _program->m_vsh->m_code, "Vertex shader doesn't exist.");
    BX_ASSERT(NULL != _layout, "Layout doesn't exist.");

    // Input layout
    D3D12_INPUT_ELEMENT_DESC vertexElements[Attrib::Count + 1 + BGFX_CONFIG_MAX_INSTANCE_DATA_COUNT];
    uint32_t countOfVertexElements = setInputLayout(vertexElements, *_layout, *_program, 0);

    // Describe and create the graphics pipeline state object (PSO)
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = {vertexElements, countOfVertexElements};
    psoDesc.pRootSignature = s_renderD3D12->m_rootSignature.Get();
    psoDesc.VS = {_program->m_vsh->m_shader->GetBufferPointer(), _program->m_vsh->m_shader->GetBufferSize()};
    psoDesc.PS = {_program->m_fsh->m_shader->GetBufferPointer(), _program->m_fsh->m_shader->GetBufferSize()};
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    HRESULT hr = s_renderD3D12->m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
    if (FAILED(hr)) {
        char errorMsg[1024];
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, hr, 0, errorMsg, sizeof(errorMsg), nullptr);
        OutputDebugStringA("Failed to create graphics pipeline state: ");
        OutputDebugStringA(errorMsg);
        OutputDebugStringA("\n");
    }
    DX_CHECK(hr);
}

} // namespace d3d12
} // namespace TinyRender

#endif // BGFX_CONFIG_RENDERER_DIRECT3D12
