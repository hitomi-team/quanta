if target_machine.system() == 'windows'
  if compiler.get_id() == 'msvc'
    unicode_main_flag = [ '/entry:wmainCRTStartup' ]
  else
    unicode_main_flag = [ '-municode' ]
  endif
else
  unicode_main_flag = []
endif

pakar = executable('pakar', files([ 'pakar.c', 'serialize.c' ]), dependencies : [], include_directories : [], link_args : [ unicode_main_flag ])
