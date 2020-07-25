subdir('null')

if get_option('d3d11') == true
	subdir('d3d11')
elif get_option('opengl') == true
	subdir('opengl')
else
	error('A renderer backend has not been set!')	
endif