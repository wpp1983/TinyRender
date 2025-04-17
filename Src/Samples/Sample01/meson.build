
executable(
  'cubes',
  'cubes.cpp',
  cpp_args: [bx_cpp_args],
  include_directories: common_headers,
  dependencies: [
    bx_dep,
    render_dep,
  ],
  link_args: [
    '-ld3d12',
    '-ldxgi',
    '-ld3dcompiler',
    '-lkernel32',
    '-luser32',
    '-lgdi32',
  ],
  win_subsystem: 'windows',
  install: true,
)