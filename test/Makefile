SRC=foo.cpp
LUA_INCLUDE=-I/usr/include/lua5.2 -I../core
LUA_LIB=-L/usr/lib/x86_64-linux-gnu/ -llua5.2
all : foo.bin 
foo.bin : $(SRC)
	g++ -g -Wall -o $@ $^  $(LUA_INCLUDE) $(LUA_LIB)
clean:
	rm -f foo.bin 
