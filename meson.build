project('quanta', ['c', 'cpp'], version : 'v0.01', meson_version : '>= 0.47', default_options : [
	'buildtype=debugoptimized',
	'b_ndebug=if-release',
	'c_std=c99',
	'cpp_std=c++14',
	'warning_level=3',
	'werror=false'
])

compiler       = meson.get_compiler('cpp')
inc_directory  = include_directories('./inc')
src_directory  = include_directories('./src')

includes = [inc_directory]
compiler_flags = []
libs = []

if get_option('debug') == true
	compiler_flags += '-D__DEBUG'
else
	compiler_flags += '-D__RELEASE'
endif

# TODO: still, manage dependencies better?
if target_machine.system() == 'windows'
	lib_path     = meson.source_root() + '/lib/win64release'
	lib_sdl2     = compiler.find_library('SDL2', dirs : lib_path)
	lib_d3d11    = compiler.find_library('d3d11', dirs : lib_path)
	lib_dxgi     = compiler.find_library('dxgi', dirs : lib_path)
	lib_d3dcomp  = compiler.find_library('d3dcompiler', dirs : lib_path)
	lib_dxguid   = compiler.find_library('dxguid', dirs : lib_path)
	lib_wincodec = compiler.find_library('windowscodecs', dirs : lib_path)

	compiler_flags += '-D__D3D11'
	libs += lib_sdl2
	libs += lib_d3d11
	libs += lib_dxgi
	libs += lib_d3dcomp
	libs += lib_dxguid
	libs += lib_wincodec

	includes += include_directories('./inc-win')
else
	lib_sdl2     = compiler.find_library('SDL2')

	libs += lib_sdl2
endif

# tools
subdir('src/physfs')
subdir('tools')

# get source files
srcs = []
subdir('src')
srcs += physfs_srcs

# libraries

compiler_flags += '-D__VULKAN'
includes += src_directory

binary = executable('quanta', srcs, dependencies : libs, include_directories : includes, c_args : compiler_flags, cpp_args : compiler_flags, cpp_pch : './src/pch/pch.h')
