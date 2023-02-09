RANLIB =       ranlib
DEFPYTHONPATH= .:../lib
CC = gcc
CFLAGS = -Wall
ODIR = obj
SRCDIR = Python
HEADDIR = Include
_OBJ = $(STANDARD_OBJ)
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))
GEN_SRC = $(patsubst %, Parser/%, $(GENSOURCES))
OBJ_SRC = $(patsubst %, Objects/%, $(OBJECTSSOURCES))
MAIN_SRC = $(patsubst %, Objects/%, $(MAINSOURCES))
OBJPILE = $(CC) $(CFLAGS) -I $(HEADDIR) $< -o $@     
COMPILE = $(CC) $(CFLAGS)    -o bin/$@.elf $^

STANDARD_OBJ=  acceler.o bltinmodule.o ceval.o classobject.o \
               compile.o dictobject.o errors.o fgetsintr.o \
               fileobject.o floatobject.o $(FMOD_OBJ) frameobject.o \
               funcobject.o $(GETCWD_OBJ) \
               graminit.o grammar1.o import.o \
               intobject.o intrcheck.o listnode.o listobject.o \
               mathmodule.o methodobject.o modsupport.o \
               moduleobject.o node.o object.o parser.o \
               parsetok.o posixmodule.o regexp.o regexpmodule.o \
               strdup.o $(STRERROR_OBJ) \
               stringobject.o $(STRTOL_OBJ) structmember.o \
               sysmodule.o timemodule.o tokenizer.o traceback.o \
               tupleobject.o typeobject.o

STANDARD_SRC=  $(GEN_SRC) $(FMOD_SRC) $(GETCWD_SRC) \
               $(OBJ_SRC) $(STRERROR_SRC) \
               $(STRTOL_SRC) $(MAIN_SRC)

GENOBJECTS=    acceler.o fgetsintr.o grammar1.o \
               intrcheck.o listnode.o node.o parser.o \
               parsetok.o strdup.o tokenizer.o bitset.o \
               firstsets.o grammar.o metagrammar.o pgen.o \
               pgenmain.o printgrammar.o

GENSOURCES=    acceler.c fgetsintr.c grammar1.c \
               intrcheck.c listnode.c node.c parser.c \
               parsetok.c strdup.c tokenizer.c bitset.c \
               firstsets.c grammar.c metagrammar.c pgen.c \
               pgenmain.c printgrammar.c

OBJECTSSOURCES=	classobject.c  floatobject.c  funcobject.c  listobject.c    moduleobject.c  stringobject.c  typeobject.c \
				fileobject.c   frameobject.c  intobject.c   methodobject.c  object.c        tupleobject.c   xxobject.c

MAINSOURCES=   bltinmodule.c  cgensupport.c  errors.c  getcwd.c    import.c      pythonmain.c  structmember.c  traceback.c \
			   ceval.c        compile.c      fmod.c    graminit.c  modsupport.c  strerror.c    sysmodule.c

CONFIGDEFS=    $(STDW_USE) $(AM_USE) $(AUDIO_USE) $(GL_USE) $(PANEL_USE) \
               '-DPYTHONPATH="$(DEFPYTHONPATH)"'

CONFIGINCLS=   $(STDW_INCL)

LIBDEPS=       libpython.a $(STDW_LIBDEPS) $(AM_LIBDEPS) \
               $(GL_LIBDEPS) $(PANEL_LIBSDEP) $(RL_LIBDEPS)

# NB: the ordering of items in LIBS is significant!
LIBS=          libpython.a $(STDW_LIBS) $(AM_LIBS) \
               $(PANEL_LIBS) $(GL_LIBS) $(RL_LIBS) -lm

LIBOBJECTS=    $(OBJ) $(STDW_OBJ) $(AM_OBJ) $(AUDIO_OBJ) \
               $(GL_OBJ) $(PANEL_OBJ)

LIBSOURCES=    $(STANDARD_SRC) $(STDW_SRC) $(AM_SRC) $(AUDIO_SRC) \
               $(GL_SRC) $(PANEL_SRC)

OBJECTS=       pythonmain.o

SOURCES=       $(LIBSOURCES) Python/pythonmain.c

# Main Targets
# ============

python:                libpython.a $(OBJECTS) $(LIBDEPS) Makefile
	$(CC) $(CFLAGS) $(OBJECTS) $(LIBS) -o @python
	mv @python python

libpython.a:   $(LIBOBJECTS)
	-rm -f @lib
	ar cr @lib $(LIBOBJECTS)
	$(RANLIB) @lib
	mv @lib libpython.a

python_gen:    $(GENOBJECTS) $(RL_LIBDEPS)
	$(CC) $(CFLAGS) $(GENOBJECTS) $(RL_LIBS) -o python_gen

$(ODIR)/%.o:  $(SRCDIR)/%.c
	 $(OBJPILE)
$(ODIR)/acceler.o:  Parser/acceler.c
	 $(OBJPILE)
