PROGRAM = i2c245
LIBNAME = lib$(PROGRAM)

SHARELIB = lib$(PROGRAM)

LIB_MAJOR = 1
LIB_MINOR = 0

SHELL = /bin/sh
CC = gcc
LD = ld
AR = ar
RM = rm -f
LN = ln -sf

DEBUG = -g -Wall
CFLAGS	= -O2 $(DEBUG)
LIBS = -lftdi
INCS = 

all: $(LIBNAME).so $(LIBNAME).a

$(LIBNAME).so: $(LIBNAME).so.$(LIB_MAJOR).$(LIB_MINOR)
	$(LN) $< $(LIBNAME).so.$(LIB_MAJOR)
	$(LN) $(LIBNAME).so.$(LIB_MAJOR) $(LIBNAME).so

$(LIBNAME).so.$(LIB_MAJOR).$(LIB_MINOR): $(LIBNAME).o
	$(CC) $^ $(CFLAGS) $(LIBS) -shared -Wl,-soname,$(LIBNAME).so.$(LIB_MAJOR) -o $@

$(LIBNAME).a: $(LIBNAME).o
	$(AR) rv $@ $<

$(LIBNAME).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INCS) -fPIC -c $< -o $@

clean:
	$(RM) $(LIBNAME)*

tags:
	etags *.f
