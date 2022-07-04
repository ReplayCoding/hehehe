project('frida-c', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

frida_dir = 'frida-gum/'
frida_dep = declare_dependency(
    include_directories: frida_dir,
    link_args: ['-L' + meson.current_source_dir() / frida_dir, '-lfrida-gum'],
  )

sdk_dep = declare_dependency(
    include_directories: [
      'source-sdk-2013/mp/src/mathlib',
      'source-sdk-2013/mp/src/public',
      'source-sdk-2013/mp/src/common',
      'source-sdk-2013/mp/src/public/tier0',
      'source-sdk-2013/mp/src/public/tier1',
      'source-sdk-2013/mp/src/tier1',
      'source-sdk-2013/mp/src',
    ],
  )

add_project_arguments('-fpermissive', language: ['cpp'])
shared_library('injectable',
           'injectable.cpp',
           dependencies: [frida_dep, sdk_dep],
           install : true)
