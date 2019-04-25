INSTALLDIR = ./
CC 		   := /usr/bin/g++
CFLAGS     += -Wall
OBJECTS    := TuringMachine.o
EXE        := TuringEmulator


.PHONY: all
all: $(EXE)


TuringEmulator: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^
	@ echo Changed files: $?


TuringMachine.o: TuringMachine.cpp
	g++ -c TuringMachine.cpp

.PHONY: clean
clean:
	$(RM) $(OBJECTS) 

.PHONE: cleanall
cleanall:
	$(RM) $(OBJECTS) $(EXE)
