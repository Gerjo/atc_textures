##################################################
# https://github.com/Gerjo/atc_textures
##################################################

# Specify a compiler:
CC=g++

# Flags, enable some stricter error checking and enable code optimization 
CFLAGS=-Wall -O2

# Output executable
OUT=aticompress


all:
	${CC} ${CFLAGS} libs/libPNG/*.c main.cpp -Llibs/ati/ -lTextureConverter -lz -o ${OUT}
	
clean:
	rm -rf *o ${OUT}