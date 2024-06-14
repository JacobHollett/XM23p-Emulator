TARGET=XM23p

SRC_FILES=main.c loadFuncs.c decodeFuncs.c debugFuncs.c

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	gcc -Wall -g -o $(TARGET) $(SRC_FILES)