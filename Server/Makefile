# STD targets (make shoudn't try to find files by the same name)
.PHONY: all clean install uninstall

TARGET = PatriaServer

SRC = 								\
	src/Main.c 						\
	src/Server.c 					\
	src/NetworkManager.c 			\
	src/Algoritms.c 				\
	src/FileHelper.c 				\
	src/WebSocketHandle.c			\
	src/Parsers/JSONParser.c 		\
	src/Parsers/WebSocketParser.c	

INCLUDE = \
	include/

# Compile & run target
all: $(TARGET)

$(TARGET):
	gcc -o $(TARGET) $(SRC) -I $(INCLUDE) -lssl -lcrypto
	mv ./PatriaServer ../PatriaServer

install:
	echo install

uninstall:
	echo uninstall

clean:
	echo clean