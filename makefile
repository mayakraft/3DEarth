LIBS=-lGL -lGLU -lglut

world : world.c
	gcc -o $@ $< $(LIBS)