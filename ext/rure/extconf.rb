require 'mkmf'
create_header
dir_config('rure')
have_library 'rure' or raise
create_makefile 'rure'