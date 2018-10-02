CXX=g++
AR=ar
CXXFLAGS=-W -Wall
LDFLAGS=

SRC=Adafruit_ADS1015.cpp
OUT=libads1015.a
OBJ=$(SRC:.cpp=.o)

all: examples

lib: $(OUT)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(AR) rcs $(OUT) $(OBJ)

examples: $(OUT)
	@(cd examples/singleended && $(MAKE))
	@(cd examples/differential && $(MAKE))
	@(cd examples/comparator && $(MAKE))

help:
	@echo "Usage: all, examples, lib, clean, mrproper"

clean:
	rm -f $(OBJ)
	@(cd examples/singleended && $(MAKE) $@)
	@(cd examples/differential && $(MAKE) $@)
	@(cd examples/comparator && $(MAKE) $@)

mrproper: clean
	rm -f $(OUT)
	@(cd examples/singleended && $(MAKE) $@)
	@(cd examples/differential && $(MAKE) $@)
	@(cd examples/comparator && $(MAKE) $@)
