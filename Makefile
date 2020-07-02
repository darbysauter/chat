CC = gcc
CFLAGS = -g
CPPFLAGS = -Wall -pedantic

bin=build/bin
obj=build/obj
lib=build/lib

all: dirs chat

dirs:
	mkdir -p $(bin)
	mkdir -p $(obj)
	mkdir -p $(lib)

chat: $(obj)/chat.o $(lib)/liblist.a $(lib)/libecdh.a
	$(CC) -o $(bin)/chat $(obj)/chat.o -L$(lib) -llist -lecdh
	ln -sfn $(bin)/chat ./chat

$(obj)/chat.o: chat.c
	$(CC) -o $(obj)/chat.o $(CFLAGS) $(CPPFLAGS) -c  chat.c

$(lib)/liblist.a: $(obj)/list_adders.o $(obj)/list_removers.o $(obj)/list_movers.o
	ar r $(lib)/liblist.a $(obj)/list_adders.o $(obj)/list_removers.o $(obj)/list_movers.o

$(obj)/list_adders.o: list_adders.c list.h
	$(CC) -o $(obj)/list_adders.o -c -I. $(CFLAGS) $(CPPFLAGS) list_adders.c

$(obj)/list_removers.o: list_removers.c list.h
	$(CC) -o $(obj)/list_removers.o -c -I. $(CFLAGS) $(CPPFLAGS) list_removers.c

$(obj)/list_movers.o: list_movers.c list.h
	$(CC) -o $(obj)/list_movers.o -c -I. $(CFLAGS) $(CPPFLAGS) list_movers.c

$(lib)/libecdh.a: $(obj)/ecdh.o
	ar r $(lib)/libecdh.a $(obj)/ecdh.o

$(obj)/ecdh.o: ecdh.c ecdh.h
	$(CC) -o $(obj)/ecdh.o -c -I. $(CFLAGS) $(CPPFLAGS) ecdh.c

.PHONY : clean
clean:
	rm -rf build chat