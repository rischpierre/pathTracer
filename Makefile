

CC = g++
CFLAGS = -Wno-deprecated -std=c++17

ifeq ($(BUILD),debug)
	CFLAGS += -g
else
	CFLAGS += -O3
endif

LFLAGS = -L/opt/USD/lib

LIBS = -ltbb -lpng -lsdf -lusd -lgf -ltf -lvt -lusdGeom -lusdShade 
INCL = -I/opt/USD/include -I/opt/Eigen

SRCS = $(wildcard sources/*.cpp) $(wildcard sources/integrators/*.cpp)
OBJS = $(SRCS:.cpp=.o)

MAIN = pathTracer

# TODO I need to put the files in a build folder
# TODO add singleThread option

all: $(MAIN)
	@echo compilation done

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCL) -o $(MAIN) $(OBJS) -Wl,-rpath=/opt/USD/lib $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCL) -c $<  -o $@


debug:
	$(MAKE) BUILD=debug

render:
	./$(MAIN) examples/simpleScene1.usda 
 
clean:
	rm $(MAIN) $(OBJS)
 
 
