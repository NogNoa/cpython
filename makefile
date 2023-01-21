CC = gcc
CFLAGS = -Wall
ODIR = obj
SRCDIR = Python
HEADDIR = Include
_OBJ = 
OBJ = $(patsubst %, $(ODIR)/%, $(_OBJ))
OBJPILE = $(CC) $(CFLAGS) -I $(HEADDIR) $< -o $@     
COMPILE = $(CC) $(CFLAGS)    -o bin/$@.elf $^

fmod: $(ODIR)/fmod.o
	$(COMPILE) -lm
pythonmain: $(ODIR)/pythonmain.o
	$(COMPILE)
$(ODIR)/pythonmain.o: $(SRCDIR)/pythonmain.c 
	$(OBJPILE)
$(ODIR)/%.h.o: $(HEADDIR)/%.h 
	$(OBJPILE)
$(ODIR)/%.o:  $(SRCDIR)/%.c
	 $(OBJPILE)
