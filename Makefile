

CC = g++
CFLAGS = -Wno-deprecated -std=c++17

LFLAGS = -L/opt/USD/lib

LIBS = -ltbb -lpng -lsdf -lusd -lgf -ltf -lvt -lusdGeom -lusdShade 
INCL = -I/opt/USD/include -I/opt/Eigen

SRCS = $(wildcard sources/*.cpp) $(wildcard sources/integrators/*.cpp)
OBJS = $(SRCS:.cpp=.o)

MAIN = pathTracer

# TODO I need to put the files in a build folder

all: $(MAIN)
	@echo compilation done

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCL) -o $(MAIN) $(OBJS) -Wl,-rpath=/opt/USD/lib $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCL) -c $<  -o $@

render:
	./$(MAIN) ../examples/simpleScene.usda 
 
clean:
	rm -f $(MAIN) $(OBJS)
 
 
