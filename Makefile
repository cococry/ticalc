CC = cc
CFLAGS = 
LIBS = -lglfw -lleif -lclipboard -lm -lGL
SRC = *.c 
BIN_NAME = ticalc

all: build 

build:
	@if [ ! -d ./vendor/leif/lib ]; then \
		$(MAKE) -C ./vendor/leif/; \
		$(MAKE) -C ./vendor/leif/ install; \
	fi
	${CC} -o ${BIN_NAME} ${SRC} ${CFLAGS} ${LIBS} 

clean:
		$(MAKE) -C ./vendor/leif/ clean
		rm -f ${BIN_NAME}

install:
	cp ${BIN_NAME} /usr/bin/
	cp ./ticalc.desktop /usr/share/applications
	cp -r ./logo /usr/share/icons/ticalc

uninstall: clean
	rm -f /usr/bin/${BIN_NAME}
	rm -f /usr/share/applications/ticalc.desktop
	rm -rf /usr/share/icons/ticalc/
