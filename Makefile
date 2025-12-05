OBJS = Scenegraphs.o View.o Controller.o Model.o Trackball.o RaySceneParser.o RayTracer.o TextureLoader.o
INCLUDES = -I../include
LIBS = -L../lib
LDFLAGS = -lglad -lglfw3
PROGRAM = Scenegraphs
CFLAGS = -g -std=c++11


ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    LDFLAGS += -lopengl32 -lgdi32
    PROGRAM :=$(addsuffix .exe,$(PROGRAM))
	COMPILER = g++
else ifeq ($(shell uname -s),Darwin)     # is MACOSX
    LDFLAGS += -framework Cocoa -framework OpenGL -framework IOKit
	COMPILER = clang++
endif

Scenegraphs: $(OBJS)
	$(COMPILER) -o $(PROGRAM) $(OBJS) $(LIBS) $(LDFLAGS)

Scenegraphs.o: Scenegraphs.cpp
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Scenegraphs.cpp

Trackball.o: Trackball.cpp Trackball.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Trackball.cpp

View.o: View.cpp View.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c View.cpp	

Controller.o: Controller.cpp Controller.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Controller.cpp	

Model.o: Model.cpp Model.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c Model.cpp

RaySceneParser.o: RaySceneParser.cpp RaySceneParser.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c RaySceneParser.cpp		
	
RayTracer.o: RayTracer.cpp RayTracer.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c RayTracer.cpp		
	
TextureLoader.o: TextureLoader.cpp TextureLoader.h
	$(COMPILER) $(INCLUDES) $(CFLAGS) -c TextureLoader.cpp	

RM = rm	-f
ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    RM := rm -f
endif

clean: 
	$(RM) $(OBJS) $(PROGRAM)

    