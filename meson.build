project('TinyRender', 'cpp', version: '0.1', default_options: ['cpp_std=c++20'])

if host_machine.cpu_family() != 'x86_64'
  error('You must build this project in a 64-bit environment (x64)')
endif

# Add /Zc:__cplusplus flag for MSVC
if meson.get_compiler('cpp').get_id() == 'msvc'
  add_project_arguments('/Zc:__cplusplus', language: 'cpp')
endif

# 公用头文件
common_headers = [
  include_directories('Src/ThirdParty/D3D12/directx'),
  include_directories('C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um'),
  include_directories('C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/shared'),
  include_directories('C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/winrt'),
]


subdir('Src/ThirdParty/bx')

subdir('Src/common')

subdir('Src/render')

subdir('Src/Samples/Sample01-HelloWorld')

subdir('Src/Samples/Sample02-EarlyDepthTest')
