CC=gcc
CFLAGS=-I.
DEPS = geometry.h geodraw.h supp.h
LIBS=-lGL -lGLU -lglut -lm

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

polytope: polytope.o geometry.o supp.o geodraw.o
	$(CC) -o polytope polytope.o geometry.o supp.o geodraw.o $(LIBS)

test:
	./polytope
	
debug:
	valgrind ./polytope
	
tesseract:
	./polytope -symbol "4 3 3"

stel120:
	./polytope -symbol "5/2 3 5"

ball:
	./polytope -symbol "5 3 3" -scale 0.25

doubletesseract:
	./polytope -symbol "4 3 3 3"
