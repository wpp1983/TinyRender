common_cpp_args = []

# Include directories
common_inc = [include_directories('.'), include_directories('entry')]

# Source files
common_src = [
    'entry/entry.cpp',
]

common_lib = static_library(
    'common',
    common_src,
    include_directories: common_inc,
    cpp_args: common_cpp_args,
    dependencies: [
        bx_dep,
    ],
      link_args: [
    '-lkernel32',
    '-luser32',
    '-lgdi32',
  ],
)

common_dep = declare_dependency(
    include_directories: common_inc,
    link_with: common_lib,
    compile_args: common_cpp_args ,
)