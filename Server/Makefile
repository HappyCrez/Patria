# STD targets (make shoudn't try to find files by the same name)
.PHONY: all clean install uninstall

TARGET = PatriaServer

SRC = \
	src/Main.c \
	src/NetworkManager.c \
	src/Algoritms.c \
	src/FileHelper.c \
	src/ParserJSON.c

INCLUDE = \
	include/

# Compile & run target
all: $(TARGET)


$(TARGET):
	gcc -o $(TARGET) $(SRC) -I $(INCLUDE)
	mv ./PatriaServer ../PatriaServer

install:
	echo install

uninstall:
	echo uninstall

clean:
	echo clean