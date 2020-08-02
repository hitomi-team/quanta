project('quanta', ['c', 'cpp'], version : 'v0.01', meson_version : '>= 0.47')

add_global_arguments('-std=c++14', language : 'cpp')
compiler       = meson.get_compiler('cpp')
inc_directory  = include_directories('./inc')

if get_option('enable_debug') == true
	add_global_arguments('-D=__DEBUG -ggdb -O2', language : 'cpp')
else
	add_global_arguments('-D=__RELEASE -O2', language : 'cpp')
endif

# get source files
subdir('src')
src = layer1_src

# libraries

# TODO: Manage dependencies better. For now, this works even though it's fucking retarded
if target_machine.system() == 'windows'
	lib_path 	= meson.source_root() + '/lib/win64release'
	lib_sdl2	= compiler.find_library('SDL2', dirs : lib_path)
	lib_sdl2main	= compiler.find_library('SDL2main', dirs : lib_path)

	if get_option('d3d11') == true
		lib_d3d11	= compiler.find_library('d3d11', dirs : lib_path)
		lib_dxgi	= compiler.find_library('dxgi', dirs : lib_path)
		lib_d3dcomp	= compiler.find_library('d3dcompiler', dirs : lib_path)
		add_global_arguments('-D=__D3D11', language : 'cpp')
		binary = executable('quanta', src, dependencies : [lib_sdl2, lib_sdl2main, lib_d3d11, lib_dxgi, lib_d3dcomp], include_directories : [inc_directory])
	elif get_option('opengl') == true
		lib_gl		= compiler.find_library('opengl32')
		lib_glew	= compiler.find_library('glew32s', dirs : lib_path)
		add_global_arguments('-D=__OPENGL', language : 'cpp')
		binary = executable('quanta', src, dependencies : [lib_sdl2, lib_sdl2main, lib_gl, lib_glew], include_directories : [inc_directory])
	endif

else # linux
	lib_sdl2	= compiler.find_library('SDL2')
	lib_sdl2main	= compiler.find_library('SDL2main')
	
	if get_option('opengl') == true
		lib_gl   = compiler.find_library('GL')
		lib_glew = compiler.find_library('GLEW')
		add_global_arguments('-D=__OPENGL', language : 'cpp')
	endif
	# TODO: Vulkan support?

	binary = executable('quanta', src, dependencies : [lib_sdl2, lib_sdl2main, lib_gl, lib_glew], include_directories : [inc_directory])
endif

subdir('tools')
