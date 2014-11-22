# Linux (default)
EXE = world
LDFLAGS = -lGL -lGLU -lglut

# Windows (cygwin)
ifeq "$(OS)" "Windows_NT"
	EXE = world.exe
	LDFLAGS = -lopengl32 -lglu32 -lglut32
endif

# OS X
# ifeq ($(OSTYPE), "darwin"*)
	LDFLAGS = -framework Carbon -framework OpenGL -framework GLUT
# endif

$(EXE) : world.c
	gcc -o $@ $< $(CFLAGS) $(LDFLAGS)