render_cpp_args = []

# Include directories
render_inc = [include_directories('.'), include_directories('rhi')]

# Source files
render_src = [
    'tiny_render.cpp',
    'rhi/rhi_d3d12.cpp',
]

render_lib = static_library(
    'render',
    render_src,
    include_directories: render_inc + common_headers,
    cpp_args: render_cpp_args,
    dependencies: [
        bx_dep,
    ],
      link_args: [
    '-ld3d12',
    '-ldxgi',
    '-ld3dcompiler',
    '-lkernel32',
    '-luser32',
    '-lgdi32',
  ],
)

render_dep = declare_dependency(
    include_directories: render_inc,
    link_with: render_lib,
    compile_args: render_cpp_args ,
)