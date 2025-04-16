#include "entry_p.h"

#if !defined(BX_PLATFORM_WINDOWS)
#define BX_PLATFORM_WINDOWS 1
#endif
#if BX_PLATFORM_WINDOWS

#include "Render/platform.h"

#include <tinystl/allocator.h>
#include <tinystl/string.h>
#include <tinystl/vector.h>

#include <windows.h>
#include <windowsx.h>
#include <xinput.h>
#include <shellapi.h>

namespace Entry
{
	typedef tinystl::vector<WCHAR> WSTRING;

	inline WSTRING UTF8ToUTF16(const char *utf8_str)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, NULL, 0);
		WSTRING utf16(len);
		MultiByteToWideChar(CP_UTF8, 0, utf8_str, -1, utf16.data(), len);
		return utf16;
	}

	inline uint32_t uint32_max(uint32_t _a, uint32_t _b)
	{
		return _a > _b ? _a : _b;
	}
	enum
	{
		WM_USER_WINDOW_CREATE = WM_USER,
		WM_USER_WINDOW_DESTROY,
		WM_USER_WINDOW_SET_TITLE,
		WM_USER_WINDOW_SET_FLAGS,
		WM_USER_WINDOW_SET_POS,
		WM_USER_WINDOW_SET_SIZE,
		WM_USER_WINDOW_TOGGLE_FRAME,
		WM_USER_WINDOW_MOUSE_LOCK,
	};

	struct Msg
	{
		Msg()
			: m_x(0)
			, m_y(0)
			, m_width(0)
			, m_height(0)
			, m_flags(0)
			, m_flagsEnabled(false)
		{
		}

		int32_t  m_x;
		int32_t  m_y;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_flags;
		tinystl::string m_title;
		bool m_flagsEnabled;
	};

	struct Context
	{
        static LRESULT CALLBACK wndProc(HWND _hwnd, UINT _id, WPARAM _wparam, LPARAM _lparam);

		HWND m_hwnd[1];
		uint32_t m_flags[1];
		RECT m_rect;
		DWORD m_style;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_oldWidth;
		uint32_t m_oldHeight;
		uint32_t m_frameWidth;
		uint32_t m_frameHeight;
		float m_aspectRatio;

		int32_t m_mx;
		int32_t m_my;
		int32_t m_mz;

		bool m_frame;
		HWND m_mouseLock;
		bool m_init;
		bool m_exit;

        int run(int _argc, const char* const* _argv)
		{
            SetDllDirectoryA(".");

            HINSTANCE instance = (HINSTANCE)GetModuleHandle(NULL);

            WNDCLASSEXW wnd;
			memset(&wnd, 0, sizeof(wnd) );
			wnd.cbSize = sizeof(wnd);
			wnd.style = CS_HREDRAW | CS_VREDRAW;
			wnd.lpfnWndProc = wndProc;
			wnd.hInstance = instance;
			wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
			wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
			wnd.lpszClassName = L"bgfx";
			wnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
			RegisterClassExW(&wnd);

            m_hwnd[0] = CreateWindowExA(
				  WS_EX_ACCEPTFILES
				, "bgfx"
				, "BGFX"
				, WS_OVERLAPPEDWINDOW|WS_VISIBLE
				, 0
				, 0
				, ENTRY_DEFAULT_WIDTH
				, ENTRY_DEFAULT_HEIGHT
				, NULL
				, NULL
				, instance
				, 0
				);

            m_flags[0] = 0
				| ENTRY_WINDOW_FLAG_ASPECT_RATIO
				| ENTRY_WINDOW_FLAG_FRAME
				;

            adjust(m_hwnd[0], ENTRY_DEFAULT_WIDTH, ENTRY_DEFAULT_HEIGHT, true);
			clear(m_hwnd[0]);

            while (!m_exit)
			{
                TinyRender::renderFrame();
            }

            DestroyWindow(m_hwnd[0]);
			return 0;
		}

        LRESULT process(HWND _hwnd, UINT _id, WPARAM _wparam, LPARAM _lparam)
		{
            if (m_init)
			{
				switch (_id)
				{
				case WM_USER_WINDOW_CREATE:
					{
						Msg* msg = (Msg*)_lparam;
						HWND hwnd = CreateWindowW(L"bgfx"
							, UTF8ToUTF16(msg->m_title.c_str()).data()
							, WS_OVERLAPPEDWINDOW|WS_VISIBLE
							, msg->m_x
							, msg->m_y
							, msg->m_width
							, msg->m_height
							, NULL
							, NULL
							, (HINSTANCE)GetModuleHandle(NULL)
							, 0
							);

						adjust(hwnd, msg->m_width, msg->m_height, true);
						clear(hwnd);

						m_hwnd[_wparam]  = hwnd;
						m_flags[_wparam] = msg->m_flags;
						WindowHandle handle = { (uint16_t)_wparam };

						delete msg;
					}
					break;

				case WM_USER_WINDOW_DESTROY:
					{
						WindowHandle handle = { (uint16_t)_wparam };
						DestroyWindow(m_hwnd[_wparam]);
						m_hwnd[_wparam] = 0;

						if (0 == handle.idx)
						{
							m_exit = true;
						}
					}
					break;
				}
			}

            return DefWindowProcW(_hwnd, _id, _wparam, _lparam);
		}

        void clear(HWND _hwnd)
		{
			RECT rect;
			GetWindowRect(_hwnd, &rect);
			HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0) );
			HDC hdc = GetDC(_hwnd);
			SelectObject(hdc, brush);
			FillRect(hdc, &rect, brush);
			ReleaseDC(_hwnd, hdc);
			DeleteObject(brush);
		}

        void adjust(HWND _hwnd, uint32_t _width, uint32_t _height, bool _windowFrame)
		{
			m_width  = _width;
			m_height = _height;
			m_aspectRatio = float(_width)/float(_height);

			ShowWindow(_hwnd, SW_SHOWNORMAL);
			RECT rect;
			RECT newrect = {0, 0, (LONG)_width, (LONG)_height};
			DWORD style = WS_POPUP|WS_SYSMENU;

			if (m_frame)
			{
				GetWindowRect(_hwnd, &m_rect);
				m_style = GetWindowLong(_hwnd, GWL_STYLE);
			}

			if (_windowFrame)
			{
				rect = m_rect;
				style = m_style;
			}
			else
			{
				HMONITOR monitor = MonitorFromWindow(_hwnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO mi;
				mi.cbSize = sizeof(mi);
				GetMonitorInfo(monitor, &mi);
				newrect = mi.rcMonitor;
				rect = mi.rcMonitor;
				m_aspectRatio = float(newrect.right  - newrect.left)/float(newrect.bottom - newrect.top);
			}

			SetWindowLong(_hwnd, GWL_STYLE, style);
			uint32_t prewidth  = newrect.right - newrect.left;
			uint32_t preheight = newrect.bottom - newrect.top;
			AdjustWindowRect(&newrect, style, FALSE);
			m_frameWidth  = (newrect.right  - newrect.left) - prewidth;
			m_frameHeight = (newrect.bottom - newrect.top ) - preheight;
			UpdateWindow(_hwnd);

			if (rect.left == -32000
			||  rect.top  == -32000)
			{
				rect.left = 0;
				rect.top  = 0;
			}

			int32_t left   = rect.left;
			int32_t top    = rect.top;
			int32_t width  = (newrect.right-newrect.left);
			int32_t height = (newrect.bottom-newrect.top);

			if (!_windowFrame)
			{
				float aspectRatio = 1.0f/m_aspectRatio;
				width  = uint32_max(ENTRY_DEFAULT_WIDTH/4, width);
				height = uint32_t(float(width)*aspectRatio);

				left   = newrect.left+(newrect.right -newrect.left-width)/2;
				top    = newrect.top +(newrect.bottom-newrect.top-height)/2;
			}

			SetWindowPos(_hwnd
				, HWND_TOP
				, left
				, top
				, width
				, height
				, SWP_SHOWWINDOW
				);

			ShowWindow(_hwnd, SW_RESTORE);

			m_frame = _windowFrame;
		}
    };

    static Context s_ctx;

    LRESULT CALLBACK Context::wndProc(HWND _hwnd, UINT _id, WPARAM _wparam, LPARAM _lparam)
	{
		return s_ctx.process(_hwnd, _id, _wparam, _lparam);
	}

	void* getNativeWindowHandle(WindowHandle _handle)
	{
		return s_ctx.m_hwnd[_handle.idx];
	}

	void* getNativeDisplayHandle()
	{
		return NULL;
	}
}

int main(int _argc, const char* const* _argv)
{
	using namespace Entry;
	return s_ctx.run(_argc, _argv);
}

#endif // BX_PLATFORM_WINDOWS
