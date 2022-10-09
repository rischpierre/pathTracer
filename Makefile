

CC = g++
CFLAGS = -Wno-deprecated -std=c++17

USD_ROOT = /opt/USD
EIGEN_ROOT = /opt/Eigen
MAIN = pathTracer
BUILD_DIR = build
SRCS_DIR = sources

ifeq ($(BUILD),debug)
	CFLAGS += -g
else
	CFLAGS += -O3
endif

ifeq ($(SINGLE_THREADED),1)
	CFLAGS += -DSINGLE_THREADED
endif

LFLAGS = -L$(USD_ROOT)/lib
LIBS = -ltbb -lpng -lsdf -lusd -lgf -ltf -lvt -lusdGeom -lusdShade 
INCL = -I$(USD_ROOT)/include -I$(EIGEN_ROOT)

SRCS = $(wildcard $(SRCS_DIR)/*.cpp)
HEADERS = $(wildcard $(SRCS_DIR)/*.h)
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all debug clean

all: $(BUILD_DIR) $(BUILD_DIR)/$(MAIN)
	@echo compilation done

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/$(MAIN): $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) $(INCL) -o $(BUILD_DIR)/$(MAIN) $(OBJS) -Wl,-rpath=$(USD_ROOT)/lib $(LFLAGS) $(LIBS)

$(OBJS): $(BUILD_DIR)/%.o: $(SRCS_DIR)/%.cpp
	$(CC) $(CFLAGS) $(INCL) -c $<  -o $@

debug:
	$(MAKE) BUILD=debug
 
debugst:
	$(MAKE) BUILD=debug SINGLE_THREADED=1

clean:
	rm -r $(BUILD_DIR)
 
