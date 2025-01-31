

TYPE_DEBUG   = debug
TYPE_RELEASE = release
# set default type
#BUILD_TYPE = $(TYPE_RELEASE)
BUILD_TYPE = $(TYPE_DEBUG)

# if DEBUG is defined set BUILD_TYPE to TYPE_DEBUG
ifdef DEBUG
BUILD_TYPE = $(TYPE_DEBUG)
endif

SRC_PATH   = ./src
BUILD_PATH = ./build

TARGET = a.out

SRC_FILES = crt.c \
            emu.c \
            mmu.c \
            mobo.c \
            raw_memory.c \
            test.c \
            z80.c \

SRCS := $(addprefix $(SRC_PATH)/,$(SRC_FILES))
OBJS := $(addprefix $(BUILD_PATH)/,$(addsuffix .o,$(SRCS)))


DEBUG_CFLAGS = -O0
DEBUG_LFLAGS = -g

RELEASE_CFLAGS = -O2
RELEASE_LFLAGS = -s

C11THREADS_CFLAGS =
C11THREADS_LFLAGS = -lstdthreads

CC = cc
WARN  = -Wall
WARN += -std=c11
WARN += -Wextra
WARN += -Wpedantic -pedantic-errors
WARN += -Waggregate-return \
		-Wbad-function-cast \
		-Wcast-align \
		-Wcast-qual \
		-Wdeclaration-after-statement \
		-Wmissing-declarations \
		-Wmissing-include-dirs \
		-Wmissing-prototypes \
		-Wnested-externs \
		-Wpointer-arith \
		-Wpointer-to-int-cast \
		-Wint-to-pointer-cast \
		-Wredundant-decls \
		-Wsequence-point \
		-Wshadow \
		-Wstrict-prototypes \
		-Wswitch \
		-Wundef \
		-Wunreachable-code \
		-Wunused-but-set-parameter \
		-Wwrite-strings \
		-fstack-protector
CFLAGS := $(C11THREADS_CFLAGS) $(WARN)

LD = cc
LFLAGS = $(C11THREADS_LFLAGS)

# BUILD_TYPE compile/linker flags
# append debug flags
ifeq ("$(BUILD_TYPE)", "$(TYPE_DEBUG)")
CFLAGS += $(DEBUG_CFLAGS)
LFLAGS += $(DEBUG_LFLAGS)
endif 

# append release flags
ifeq ("$(BUILD_TYPE)", "$(TYPE_RELEASE)")
CFLAGS += $(RELEASE_CFLAGS)
LFLAGS += $(RELEASE_LFLAGS)
endif


# names
build: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	-rmdir $(BUILD_PATH)

bear: clean
	bear --force-wrapper -- $(MAKE) build

bearclean:
	rm -f compile_commands.json


fullclean: bearclean clean

debug:
	@echo "BUILD_TYPE: $(BUILD_TYPE)"
	@echo "TARGET: $(TARGET)"
	@echo "SRCS:   $(SRCS)"
	@echo "OBJS:   $(OBJS)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LFLAGS: $(LFLAGS)"

# target
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $(OBJS) $(LFLAGS)

emu.c.o: emu.c emu.h mobo.h mmu.h Makefile
crt.c.o: crt.c crt.h mobo.h Makefile
mmu.c.o: mmu.c mmu.h raw_memory.h Makefile
mobo.c.o: mobo.c mobo.h adv_system_config.h crt.h mmu.h z80.h Makefile
raw_memory.c.o: raw_memory.c raw_memory.h Makefile 
test.c.o: test.c emu.h Makefile
z80.c.o: z80.c z80.h Makefile

# generic
$(BUILD_PATH)/$(SRC_PATH)/%.c.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS)


# vim: noet
# end of file
