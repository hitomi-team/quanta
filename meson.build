project('quanta', ['c', 'cpp'], version : 'v0.01', meson_version : '>= 0.47')

compiler_flags = '-std=c++14'
compiler       = meson.get_compiler('cpp')
inc_directory  = include_directories('./inc')

if get_option('debug') == true
	compiler_additional_flags = '-D=__DEBUG -ggdb -O2'
else
	compiler_additional_flags = '-O2'
endif

# get source files
subdir('src')
src = layer1_src

# libraries
if target_machine.system() == 'windows'
	lib_path 	= meson.source_root() + '/lib/win64release'
	lib_sdl2	= compiler.find_library('SDL2', dirs : lib_path)
	lib_sdl2main	= compiler.find_library('SDL2main', dirs : lib_path)
else # linux
	error('Will support Linux later..')
endif

binary = executable('quanta', src, dependencies : [lib_sdl2, lib_sdl2main], include_directories : [inc_directory])
