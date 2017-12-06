BIN=nist_kats
SRCPATH=../Reference_Implementation/
SRC_FILES=$(wildcard $(SRCPATH)/*.c) $(wildcard $(SRCPATH)/primitives/*.c)
SRC=$(filter-out $(SRCPATH)/randombytes.c $(SRCPATH)/bench.c, $(SRC_FILES))
SRC+=PQCgenKAT_sign.c rng.c

CFLAGS=-O3 -march=native -Wall -Wextra -pedantic -std=c99
LDFLAGS=-lcrypto

CFLAGS += -I$(SRCPATH)

KERNEL_NAME := $(shell uname -s)
ifeq ($(KERNEL_NAME), Darwin)
	CFLAGS += -I/usr/local/opt/openssl/include
	LDFLAGS += -L/usr/local/opt/openssl/lib
endif

# setting CRYPTO_BYTES to the static structure size, for simplicity,
# as opposed to the actual message-dependent signature size

VERSION_S= -DPORS_k=24 -DMERKLE_h=5 -DGRAVITY_d=1  -DGRAVITY_c=10 -DCRYPTO_BYTES=15728 -DCRYPTO_SECRETKEYBYTES=65568
VERSION_M= -DPORS_k=32 -DMERKLE_h=5 -DGRAVITY_d=7  -DGRAVITY_c=15 -DCRYPTO_BYTES=34064 -DCRYPTO_SECRETKEYBYTES=2097184
VERSION_L= -DPORS_k=28 -DMERKLE_h=5 -DGRAVITY_d=10 -DGRAVITY_c=14 -DCRYPTO_BYTES=38768 -DCRYPTO_SECRETKEYBYTES=1048608

VERSION=$(VERSION_S)

CFLAGS += $(VERSION) -DCRYPTO_PUBLICKEYBYTES=32

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $? $(LDFLAGS)
	./$(BIN)

clean:
	rm -rf $(BIN) *.o

.PHONY: all $(BIN)
