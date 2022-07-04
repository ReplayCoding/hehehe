project('frida-c', 'cpp',
  version : '0.1',
  default_options :
    ['warning_level=1', 'cpp_std=c++20']
)

frida_dir = 'frida-gum/'
frida_dep = declare_dependency(
    include_directories: frida_dir,
    link_args: ['-L' + meson.current_source_dir() / frida_dir, '-lfrida-gum'],
  )

sdk_dir = 'source-sdk-2013/'
sdk_dep = declare_dependency(
    compile_args: ['-include' + meson.current_source_dir() / 'sdkfix.hpp'],
    include_directories: [
      sdk_dir / 'mp/src/mathlib',
      sdk_dir / 'mp/src/public',
      sdk_dir / 'mp/src/common',
      sdk_dir / 'mp/src/public/tier0',
      sdk_dir / 'mp/src/public/tier1',
      sdk_dir / 'mp/src/tier1',
      sdk_dir / 'mp/src',
    ],
  )

add_project_arguments('-fpermissive', language: ['cpp'])
shared_library('injectable',
           'main.cpp',
           dependencies: [frida_dep, sdk_dep],
           install : true)
