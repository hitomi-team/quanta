project('quanta', ['c', 'cpp'], version : 'v0.01', meson_version : '>= 0.47')

compiler_flags = '-std=c++14'
compiler       = meson.get_compiler('cpp')
include_path   = include_directories('inc')

if get_option('debug') == true
	compiler_additional_flags = '-D=__DEBUG -ggdb -O2'
else
	compiler_additional_flags = '-O2'
endif

# get source files
subdir('src')
src = layer1_src

# libraries
lib_sdl2 = compiler.find_library('SDL2')

binary = executable('quanta', src, cpp_args : [compiler_flags, compiler_additional_flags], dependencies : [lib_sdl2], include_directories : [include_path])
