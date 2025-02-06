
SRC_PATH       := ./src
EXTERNAL_PATH  := ./external
BUILD_PATH     := ./build
BUILD_OBJ_PATH := $(BUILD_PATH)/objs
BUILD_DEP_PATH := $(BUILD_PATH)/deps
BUILD_BIN_PATH := $(BUILD_PATH)/bin

RELEASE_CANDIDATE = debug
OS := $(shell uname)

# Linker/Compiler
CC = cc
LD = cc

# Release vs Debug flags
ifeq ($(RELEASE_CANDIDATE),debug)
    OPT   = -g -O0 -DDEBUG
    STRIP = -g
endif
ifeq ($(RELEASE_CANDIDATE),release)
    OPT   = -O2
    STRIP = -s
endif

# C11 threads library
ifeq ($(OS),Linux)
    THREADS_LFLAGS = -lthreads
endif
ifeq ($(OS),FreeBSD)
    THREADS_LFLAGS = -lstdthreads
endif

# global flags
WARN  = -Wall 
WARN += -Wextra 
WARN += -Wpedantic -pedantic-errors
WARN += -Waggregate-return
WARN += -Wbad-function-cast
WARN += -Wcast-align
WARN += -Wcast-qual
WARN += -Wdeclaration-after-statement
WARN += -Wmissing-declarations
WARN += -Wmissing-include-dirs
WARN += -Wmissing-prototypes
WARN += -Wnested-externs
WARN += -Wpointer-arith
WARN += -Wpointer-to-int-cast
WARN += -Wint-to-pointer-cast
WARN += -Wredundant-decls
WARN += -Wsequence-point
WARN += -Wshadow
WARN += -Wstrict-prototypes
WARN += -Wswitch
WARN += -Wundef
WARN += -Wunreachable-code
WARN += -Wunused-but-set-parameter
WARN += -Wwrite-strings
WARN += -fstack-protector

# ERROR = -Werror

CFLAGS = $(WARN) $(ERROR) $(OPT) 
LFLAGS = $(STRIP)

BEAR = bear --force-wrapper
BEAR_OUTPUT_FILE = compile_command.json

# vim: noet
