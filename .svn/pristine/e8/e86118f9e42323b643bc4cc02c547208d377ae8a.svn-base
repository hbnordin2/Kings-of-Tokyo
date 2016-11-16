CC = gcc
CFLAGS = -g -Wall -pedantic -std=gnu99
TARGETS = stlucia EAIT SCIENCE HABS MABS HASS

.DEFAULT: all

all: $(TARGETS)

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c -o utilities.o

player.o: player.c utilities.h playerFunctionality.h
	$(CC) $(CFLAGS) -c player.c -o player.o

eait.o: eait.c playerFunctionality.h
	$(CC) $(CFLAGS) -c eait.c -o eait.o

stLucia.o: stLucia.c utilities.h
	$(CC) $(CFLAGS) -c stLucia.c -o stLucia.o

habs.o: habs.c
	$(CC) $(CFLAGS) -c habs.c -o habs.o

mabs.o: mabs.c
	$(CC) $(CFLAGS) -c mabs.c -o mabs.o

hass.o: hass.c
	$(CC) $(CFLAGS) -c hass.c -o hass.o

science.o: science.c playerFunctionality.h
	$(CC) $(CFLAGS) -c science.c -o science.o

EAIT: player.o eait.o playerFunctionality.h utilities.o
	$(CC) $(CFLAGS) eait.o player.o utilities.o -o EAIT

SCIENCE: player.o science.o playerFunctionality.h utilities.o
	$(CC) $(CFLAGS) science.o player.o utilities.o -o SCIENCE

HABS: habs.o player.o playerFunctionality.h utilities.o
	$(CC) $(CFLAGS) habs.o player.o utilities.o -o HABS

MABS: mabs.o player.o playerFunctionality.h utilities.o
	$(CC) $(CFLAGS) mabs.o player.o utilities.o -o MABS

HASS: hass.o player.o playerFunctionality.h utilities.o
	$(CC) $(CFLAGS) hass.o player.o utilities.o -o HASS

stlucia: stLucia.c eait.o utilities.o utilities.h
	$(CC) $(CFLAGS) stLucia.c utilities.o -o stlucia
