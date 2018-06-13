require 'mkmf'
$LDFLAGS << " -Wl,-rpath,/usr/local/lib"
create_header
dir_config('rure')
have_library 'rure' or raise
create_makefile 'rure'