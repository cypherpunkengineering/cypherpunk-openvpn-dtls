LIBS=
CPP=g++
CC=gcc

COFLAGS=-O2
# CWFLAGS=-Wall -Werror
CWFLAGS=-Wall 
CIFLAGS=-I. -I../common  -I/home/maemigh/openssl/include
#Uncomment this if you want more debug messages
#CMFLAGS=-DDEBUG 
CMFLAGS=
CLIBS=/home/maemigh/openssl/lib/libcrypto.so /home/maemigh/openssl/lib/libssl.so

CFLAGS= $(CWFLAGS) $(COFLAGS) $(CIFLAGS) $(CMFLAGS) -Wl,-rpath=/home/maemigh/openssl/lib

all: dtls_client

dtls_client.o: dtls_client.c
	$(CC) $(CFLAGS) -c dtls_client.c
sslmgr.o: ../common/sslmgr.h ../common/sslmgr.c
	$(CC) $(CFLAGS) -c ../common/sslmgr.c
	
dtls_client: dtls_client.o sslmgr.o
	$(CC) $(CFLAGS) -o dtls_client dtls_client.o sslmgr.o $(CLIBS)
	$(RM) *.o

install: compile

clean:
	$(RM) -f *.o dtls_client
		
distclean: clean
