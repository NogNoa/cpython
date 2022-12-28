CC = gcc
CFLAGS = -Wall -g
ODIR = obj
SRCDIR = Python
HEADDIR = Include
_OBJ = 
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))
OBJPILE = $(CC) $(CFLAGS) -c -o $@     $<
COMPILE = $(CC) $(CFLAGS)    -o bin/$@.elf $^

fmod: $(ODIR)/fmod.o
	$(COMPILE) -lm
pythonmain: $(ODIR)/pythonmain.o
	$(COMPILE)
$(ODIR)/pythonmain.o: $(SRCDIR)/pythonmain.c $(HEADDIR)/allobjects.h
	$(OBJPILE)
$(ODIR)/%.h.o: $(HEADDIR)/%.h 
	$(OBJPILE)
$(ODIR)/%.o:  $(SRCDIR)/%.c
	 $(OBJPILE)
