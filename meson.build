project('TinyRenderer', 'cpp', default_options : ['cpp_std=c++20'])

src = files(
  'src/Core/commandline.cpp',
  'src/Core/core.cpp',
  'src/RHI/D3D12/gfx_dx12.cpp',
  'src/Common/entry/entry.cpp',
  'src/Common/entry/entry_windows.cpp',
  'src/Render/TinyRender.cpp',
  'src/Common/util.cpp',
  'src/Samples/Sample01/cubes.cpp',
)

incdir = include_directories('src', 'src/ThirdParty')

executable('TinyRenderer_dx12', src, include_directories : incdir,
           cpp_args: ['-DUNICODE', '-D_WIN32_WINNT=0x0A00'],
          #  win_subsystem: 'windows',
           link_args: ['-ld3d12', '-ldxgi', '-ldxguid', '-ld3dcompiler'])
