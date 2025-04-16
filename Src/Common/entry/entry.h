#pragma once

#include <stdint.h>

#include "defines.h"

extern "C" int _main_(int _argc, char **_argv);

#define ENTRY_WINDOW_FLAG_NONE UINT32_C(0x00000000)
#define ENTRY_WINDOW_FLAG_ASPECT_RATIO UINT32_C(0x00000001)
#define ENTRY_WINDOW_FLAG_FRAME UINT32_C(0x00000002)


#define ENTRY_IMPLEMENT_MAIN(_app, ...)                                        \
  int _main_(int _argc, char **_argv) {                                        \
    _app app(__VA_ARGS__);                                                     \
    return Entry::runApp(&app, _argc, _argv);                                  \
  }


///
#define ENTRY_HANDLE(_name)                                                    \
  struct _name {                                                               \
    uint16_t idx;                                                              \
  };                                                                           \
  inline bool isValid(_name _handle) { return UINT16_MAX != _handle.idx; }

namespace Entry {

ENTRY_HANDLE(WindowHandle);

///
constexpr WindowHandle kDefaultWindowHandle = { 0 };

///
WindowHandle createWindow(int32_t _x, int32_t _y, uint32_t _width,
                          uint32_t _height,
                          uint32_t _flags = ENTRY_WINDOW_FLAG_NONE,
                          const char *_title = "");

///
void destroyWindow(WindowHandle _handle);

///
void setWindowPos(WindowHandle _handle, int32_t _x, int32_t _y);

///
void setWindowSize(WindowHandle _handle, uint32_t _width, uint32_t _height);

///
void setWindowTitle(WindowHandle _handle, const char *_title);

///
void setWindowFlags(WindowHandle _handle, uint32_t _flags, bool _enabled);

///
void *getNativeWindowHandle(WindowHandle _handle);

///
void *getNativeDisplayHandle();

class AppInterface {
public:
  AppInterface(const char *_name, const char *_description, const char *_url);
  virtual ~AppInterface() ;

  virtual void Initialize(int32_t _argc, const char *const *_argv, uint32_t _width,
                          uint32_t _height) = 0;
  virtual void Update() = 0;
  virtual void Shutdown() = 0;

  		///
		const char* getName() const;

		///
		const char* getDescription() const;

		///
		const char* getUrl() const;

private:
		const char* m_name;
		const char* m_description;
		const char* m_url;

};

int runApp(AppInterface *_app, int _argc, const char *const *_argv);
} // namespace Entry
