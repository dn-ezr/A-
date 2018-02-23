
CXX_OPT = -g -std=gnu++14 -I ../inc -L ../lib

OUT_DIR = ./bin
SRC = $(wildcard *.cpp)
OUT = $(SRC:test_%.cpp=$(OUT_DIR)/%)

all:../lib/libap.a $(OUT)

$(OUT):$(OUT_DIR)/%:test_%.cpp ../lib/libap.a
	g++ $(CXX_OPT) -o $@  $< -lap

clean:
	rm -rf ./bin/*

.PONY:clean