
CXX_OPT = -c -g -std=gnu++14 -I ../inc
OBJ_DIR = ../obj
OUT_DIR = ../lib

SRC = $(wildcard *.cpp)
OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: $(OUT_DIR)/libap.a

clean:
	rm -rf $(OBJ_DIR)/*.o

$(OBJ):$(OBJ_DIR)/%.o:%.cpp
	g++ $(CXX_OPT) -o $@  $<

$(OUT_DIR)/libap.a:$(OBJ)
	ar rcsv $(OUT_DIR)/libap.a $(OBJ_DIR)/*.o