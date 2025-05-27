# STD targets (make shoudn't try to find files by the same name)
.PHONY: all clean install uninstall

TARGET = PatriaServer

SRC = 							\
	src/main.c 					\
	src/server.c 					\
	src/network_manager.c 				\
	src/algoritms/base64_coder.c 			\
	src/algoritms/bst.c				\
	src/algoritms/common_base_algoritms.c		\
	src/file_helper.c 				\
	src/web_socket_handler.c			\
	src/parsers/json_parser.c 			\
	src/parsers/web_socket_parser.c			\
	src/parsers/web_socket_builder.c

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

# Think about testing the app
TEST_SRC = 				\
	tests/test_json_parser.c 	

test:
	gcc -o tester $(SRC) $(TEST_SRC) -I $(INCLUDE) -lssl -lcrypto