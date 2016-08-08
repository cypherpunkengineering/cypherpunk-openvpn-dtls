 #  
### Makefile for server
 #

STRIP = strip

SHELL= /bin/sh
LIBS = /home/maemigh/openssl/lib/libcrypto.so /home/maemigh/openssl/lib/libssl.so

# Use this when you want DEBUG info
#CFLAGS = -DLINUX -D_REENTRANT -pipe -g -O2 -Wall -Dlint -funroll-loops -fomit-frame-pointer -I../common -I -DDEBUG
# CFLAGS = -DLINUX -D_REENTRANT -pipe -g -O2 -Wall -Werror -Dlint -funroll-loops -fomit-frame-pointer -I../common -I/usr/local/include/openssl 
CFLAGS = -DLINUX -D_REENTRANT -pipe -g -O2 -Wall -Dlint -funroll-loops -fomit-frame-pointer -I../common -I/home/maemigh/openssl/include
LDFLAGS = -Wl,-rpath=/home/maemigh/openssl/lib 

all: dtls_server

dtls_server.o: dtls_server.c
	$(CC) $(CFLAGS) -c dtls_server.c
sslmgr.o: ../common/sslmgr.c ../common/sslmgr.h
	$(CC) $(CFLAGS) -c ../common/sslmgr.c

dtls_server: dtls_server.o sslmgr.o
	
	$(CC) $(CFLAGS) $(LDFLAGS) -o dtls_server sslmgr.o dtls_server.o $(LIBS)
	
	$(STRIP) dtls_server	
	$(RM) -f *.o

install: all

clean:
	$(RM) -f *.o dtls_server

