# Makefile to build class 'xfade~' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

ldflags = -L/usr/local/lib/ -lfftw3f -lm


# library name
lib.name = hrtf~

# input source file (class name == source file basename)
class.sources = hrtf~.c

# all extra files to be included in binary distribution of the library
datafiles = 

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'

include ./Makefile.pdlibbuilder
