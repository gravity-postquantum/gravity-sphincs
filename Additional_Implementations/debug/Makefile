SRC=$(wildcard *.c) $(wildcard primitives/*.c) 
OBJ=$(patsubst %.c, %.o, $(SRC))
IVSFILE=IntermediateValues.txt
BIN=gen_ivs


CFLAGS_COMMON=-std=c89 -pedantic -Wall -Wextra -Wno-long-long -march=native
LDFLAGS=-lcrypto

VERSION_S= -DPORS_k=24 -DMERKLE_h=5  -DGRAVITY_d=1  -DGRAVITY_c=10
VERSION_M= -DPORS_k=32 -DMERKLE_h=5  -DGRAVITY_d=7  -DGRAVITY_c=15
VERSION_L= -DPORS_k=28 -DMERKLE_h=5  -DGRAVITY_d=10 -DGRAVITY_c=14 

VERSION=$(VERSION_S)

CFLAGS_COMMON+=$(VERSION)
CFLAGS=$(CFLAGS_COMMON) -O3 -fomit-frame-pointer
CFLAGS_SANITIZED=$(CFLAGS_COMMON) -fsanitize=address,undefined -g

KERNEL_NAME := $(shell uname -s)
ifeq ($(KERNEL_NAME), Darwin)
	CFLAGS += -I/usr/local/opt/openssl/include 
	LDFLAGS += -L/usr/local/opt/openssl/lib
endif

all: help

help:
			@echo "Please choose a target:"
			@echo "\tanalyze\t\t runs static analyzers"
			@echo "\tcheck\t\t verifies intermediate values"
			@echo "\tclean\t\t cleans up"
			@echo "\tformat\t\t formats the code using .clang-format rules"
			@echo "\tivs\t\t generates intermediate values"

check:		$(SRC) $(IVSFILE)
			@$(CC) $(CFLAGS) $(LDFLAGS) -DDEBUG $(SRC) -o $(BIN)
			@./gen_ivs > $(IVSFILE)_tmp
			@diff $(IVSFILE) $(IVSFILE)_tmp ; if [ $$? -ne 0 ] ; then echo "error" ; else echo "ok" ; fi
			@rm -f $(IVSFILE)_tmp

ivs:		$(SRC) gen_ivs.c
			$(CC) $(CFLAGS) $(LDFLAGS) -DDEBUG $^ -o $(BIN)
			./$(BIN) > $(IVSFILE)

format: 
			clang-format -i *.c *.h

analyze:
			cppcheck . --std=c89
			scan-build gcc -c $(CFLAGS_SANITIZED) *.c
			cppclean .
			rm -f *.o

cloc:		
			cloc $(wildcard *.c) $(wildcard *.h)

clean:
			rm -f $(BIN) *.dSYM 

.PHONY: clean format analyze cloc 

