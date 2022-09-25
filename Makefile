

CC = g++
CFLAGS = -Wno-deprecated -std=c++17

ifeq ($(BUILD),debug)
	CFLAGS += -g
else
	CFLAGS += -O3
endif

ifeq ($(SINGLE_THREADED),1)
	CFLAGS += -DSINGLE_THREADED
endif

LFLAGS = -L/opt/USD/lib
USD_LIB = /opt/USD/lib
LIBS = -ltbb -lpng -lsdf -lusd -lgf -ltf -lvt -lusdGeom -lusdShade 
INCL = -I/opt/USD/include -I/opt/Eigen

MAIN = pathTracer
BUILD_DIR = build
SRCS_DIR = sources

SRCS = $(wildcard $(SRCS_DIR)/*.cpp)
HEADERS = $(wildcard $(SRCS_DIR)/*.h)
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# TODO bug when a file is changed, the make command does not recompiles

.PHONY: all debug clean

all: $(BUILD_DIR) $(BUILD_DIR)/$(MAIN)
	@echo compilation done

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(MAIN): $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(INCL) -o $(BUILD_DIR)/$(MAIN) $(OBJS) -Wl,-rpath=$(USD_LIB) $(LFLAGS) $(LIBS)

$(OBJS): $(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCL) -c $<  -o $@

debug:
	$(MAKE) BUILD=debug
 
debugst:
	$(MAKE) BUILD=debug SINGLE_THREADED=1

clean:
	rm -r $(BUILD_DIR)
 
