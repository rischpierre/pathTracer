

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

SRCS = $(wildcard sources/*.cpp)
OBJS := $(subst: /sources/, /build/, $(SRCS:.cpp=.o))

MAIN = pathTracer
BUID_DIR = build

# TODO I need to put the files in a build folder
# TODO add singleThread option

.PHONY: all debug clean

all: $(BUILD_DIR) $(MAIN)
	@echo compilation done

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCL) -o $(MAIN) $(OBJS) -Wl,-rpath=/opt/USD/lib $(LFLAGS) $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCL) -c $<  -o $@

debug:
	$(MAKE) BUILD=debug
 
clean:
	rm $(MAIN) $(OBJS)
	rm -r $(BUILD_DIR)
 
 
