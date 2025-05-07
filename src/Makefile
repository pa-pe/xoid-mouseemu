NDK_PATH := $(HOME)/Android/Sdk/ndk/29.0.13113456
TOOLCHAIN := $(NDK_PATH)/toolchains/llvm/prebuilt/linux-x86_64
API_LEVEL := 21

CC := $(TOOLCHAIN)/bin/armv7a-linux-androideabi$(API_LEVEL)-clang

CFLAGS := -Wall -fPIE
LDFLAGS := -pie -Wl,-z,nocopyreloc

//CFLAGS := --target=armv7a-linux-androideabi33 -fPIE -pie -DANDROID -Wall -Wextra
//LDFLAGS := -llog

SRC := main.c daemon.c socket.c utils.c virtual.c mouse.c log.c

OBJ := $(SRC:.c=.o)
OUT := xoid_mouseemu

all: $(OUT)

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(OUT)
