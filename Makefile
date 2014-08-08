SRC=core/nbcore.cpp \
	core/nblua.cpp \
	core/nbactor.cpp \
	core/scheduler.cpp \
	core/nbthread.cpp 

LUALIB=-L/usr/local/lib -llua
LUAINC=-I/usr/local/include
all : knewbi.so
knewbi.so : $(SRC)
	g++ -g -Wall --shared -fPIC -o $@ $^ $(LUAINC) -lpthread
clean:
	rm -f knewbi.so 
