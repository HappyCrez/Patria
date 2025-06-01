# STD targets (make shoudn't try to find files by the same name)
.PHONY: all clean install uninstall

TARGET = PatriaServer

SRC = 								\
	src/main.c 						\
	src/server.c 						\
	src/manager.c 						\
	src/utils/bst.c						\
	src/utils/string.c					\
	src/utils/base64.c 					\
	src/utils/file_tool.c					\
	src/protocols/json_parser.c 				\
	src/protocols/ws_protocol/ws_builder.c			\
	src/protocols/ws_protocol/ws_parser.c			\
	src/protocols/ws_protocol/ws_handler.c			\
	src/protocols/http_protocol/http_builder.c 		\
	src/protocols/http_protocol/http_handler.c		

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