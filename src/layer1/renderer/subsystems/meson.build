subdir('null')
subdir('vulkan')

subsystem_src = subsystem_null_src + subsystem_vulkan_src

if target_machine.system() == 'windows'
	subdir('d3d11')
	subsystem_src = subsystem_src + subsystem_d3d11_src
endif
