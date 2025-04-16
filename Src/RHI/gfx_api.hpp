#pragma once

class GfxDevice {
public:
    virtual bool initialize(void* hwnd) = 0;
    virtual void draw_frame() = 0;
    virtual void shutdown() = 0;
    virtual ~GfxDevice() = default;
};

GfxDevice* create_dx12_device();
