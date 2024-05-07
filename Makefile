CC = cc
CFLAGS = 
LIBS = -lglfw -lleif -lclipboard -lm -lGL
SRC = *.c 
BIN_NAME = ticalc

all: build 

build:
	${CC} -o ${BIN_NAME} ${SRC} ${CFLAGS} ${LIBS} 

clean:
		rm -f ${BIN_NAME}

install:
	cp ${BIN_NAME} /usr/bin/
