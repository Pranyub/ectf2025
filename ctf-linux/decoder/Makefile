CC=gcc

VPATH += .
VPATH += src
VPATH += wolfssl/wolfcrypt/src
VPATH += wolfssl


IPATH += .
IPATH += include
IPATH += wolfssl

IPATH+=../deployment
IPATH+=inc/
IPATH+=../secrets
IPATH+=tests

VPATH := $(VPATH)
IPATH := $(IPATH)

PROJ_CFLAGS += -Wall # Enable warnings
PROJ_CFLAGS += -DNO_WOLFSSL_DIR
PROJ_CFLAGS += -DWOLFSSL_AES_DIRECT
PROJ_CFLAGS += -DSINGLE_THREADED
PROJ_CFLAGS += -DHAVE_PK_CALLBACKS
PROJ_CFLAGS += -DWOLFSSL_USER_IO
PROJ_CFLAGS += -DNO_WRITEV -DTIME_T_NOT_64BIT
PROJ_CFLAGS += -fdiagnostics-color=always

PROJ_CFLAGS += -DHAVE_CHACHA  # compile with CHACHA
PROJ_CFLAGS += -DHAVE_POLY1305  # compile with CHACHA
PROJ_CFLAGS += -DECC_TIMING_RESISTANT  # compile ECC with timing attack resistance
PROJ_CLFAGS += -DTFM_TIMING_RESISTANT
PROJ_CFLAGS += -DNO_RSA
# PROJ_CFLAGS += -DNO_SKID  # linux undefined linker error
PROJ_CFLAGS += -DWOLFCRYPT_ONLY  # linux undefined linker error

PROJ_CFLAGS += -DWOLFSSL_PUBLIC_MP
PROJ_CFLAGS += -DHAVE_ECC
PROJ_CFLAGS += -DHAVE_ECC_VERIFY
PROJ_CFLAGS += -DWOLFSSL_CUSTOM_CURVES
PROJ_CFLAGS += -DHAVE_ECC_KOBLITZ

# Collect all source files correctly
SRCS := $(foreach dir, $(VPATH), $(wildcard $(dir)/*.c))

# Convert .c files to .o files
OBJS := $(SRCS:.c=.o)

# Convert IPATH into -I flags for compiler
INC_FLAGS := $(foreach dir, $(IPATH), -I$(dir))

.PHONY: all clean

TARGET := decoder

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) -lm

%.o: %.c
	$(CC) $(PROJ_CFLAGS) $(INC_FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
