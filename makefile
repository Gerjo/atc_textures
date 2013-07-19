CC=g++
CFLAGS=-Wall -O2

OUT=aticompress

all:
	${CC} ${CFLAGS} libs/libPNG/*.c main.cpp -Llibs/ati/ -lTextureConverter -lz -o ${OUT}
	
clean:
	rm -rf *o ${OUT}