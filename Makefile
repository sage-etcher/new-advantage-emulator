
include config.mk



# Path declaration
# Helps for sharing includes
ADV_PATH    := $(SRC_PATH)
WIN_PATH    := $(SRC_PATH)
Z80EMU_PATH := $(EXTERNAL_PATH)/z80emu

# Z80Emu
Z80EMU_FILES  := z80emu.c
Z80EMU_SRCS   := $(addprefix $(Z80EMU_PATH)/,$(Z80EMU_FILES))
Z80EMU_OBJS   := $(foreach filename,$(Z80EMU_FILES),$(BUILD_OBJ_PATH)/z80emu/$(filename).o)
Z80EMU_DEPS   := $(foreach filename,$(Z80EMU_FILES),$(BUILD_DEP_PATH)/z80emu/$(filename).d)
Z80EMU_CFLAGS := -I$(Z80EMU_PATH) -I$(ADV_PATH)
Z80EMU_USER_EX    := $(Z80EMU_PATH)/z80user.h
Z80EMU_USER_NOUSE := $(Z80EMU_USER_EX).nouse

# Advantage Emulator
ADV_TARGET := $(BUILD_BIN_PATH)/a.out
ADV_FILES  := \
			  control_panel.c \
			  crt.c \
			  emu.c \
			  fileio.c \
			  mmu.c \
			  mobo.c \
			  raw_memory.c \
			  test.c \
			  timer.c \
			  z80.c
ADV_SRCS   := $(addprefix $(ADV_PATH)/,$(ADV_FILES))
ADV_DEPS   := $(foreach filename,$(ADV_FILES),$(BUILD_DEP_PATH)/adv/$(filename).d)
ADV_OBJS   := $(foreach filename,$(ADV_FILES),$(BUILD_OBJ_PATH)/adv/$(filename).o) \
			  $(Z80EMU_OBJS)
ADV_CFLAGS := -I$(ADV_PATH) -I$(Z80EMU_PATH)
ADV_LFLAGS := $(THREADS_LFLAGS)

# GTK4 test
WIN_TARGET := $(BUILD_BIN_PATH)/b.out
WIN_FILES  := main_window.c
WIN_SRCS   := $(addprefix $(WIN_PATH)/,$(WIN_FILES))
WIN_DEPS   := $(foreach filename,$(WIN_FILES),$(BUILD_DEP_PATH)/win/$(filename).d)
WIN_OBJS   := $(foreach filename,$(WIN_FILES),$(BUILD_OBJ_PATH)/win/$(filename).o)
WIN_CFLAGS := -I$(WIN_PATH) $(GTK4_CFLAGS)
WIN_LFLAGS := $(GTK4_LFLAGS)


# Collections
TARGETS := $(ADV_TARGET) $(WIN_TARGET)



# Phonys
all: build

build: $(TARGETS)

clean:
	rm -f $(ADV_OBJS)
	rm -f $(ADV_DEPS)
	rm -f $(ADV_TARGET)
	rm -f $(Z80EMU_OBJS)
	rm -f $(Z80EMU_DEPS)
	-mv $(Z80EMU_USER_NOUSE) $(Z80EMU_USER_EX)
	rm -f $(WIN_OBJS)
	rm -f $(WIN_DEPS)
	rm -f $(WIN_TARGET)
	-find $(BUILD_PATH) -type d -delete

runadv:
	$(ADV_TARGET)

runwin:
	$(WIN_TARGET)

bear: $(BEAR_OUTPUT_FILE)

cleanbear:
	rm -f $(BEAR_OUTPUT_FILE)

$(BEAR_OUTPUT_FILE):
	$(MAKE) clean
	$(BEAR) -- $(MAKE) build

fullclean: clean cleanbear

debug:
	@echo "path $(Z80EMU_PATH)"
	@echo "files $(Z80EMU_FILES)"
	@echo "srcs $(Z80EMU_SRCS)"
	@echo "objs $(Z80EMU_OBJS)"
	@echo "deps $(Z80EMU_DEPS)"
	@echo "cflags $(Z80EMU_CFLAGS)"


# advantage stuff
$(ADV_TARGET): $(ADV_OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $(ADV_OBJS) $(LFLAGS) $(ADV_LFLAGS)

$(BUILD_OBJ_PATH)/adv/%.c.o: $(ADV_PATH)/%.c
	@mkdir -p $(dir $@)
	@$(CC) -c -o $@ $< $(CFLAGS) $(ADV_CFLAGS)

$(BUILD_DEP_PATH)/adv/%.c.d: $(ADV_PATH)/%.c
	@mkdir -p $(dir $@)
	@rm -f $@
	@$(CC) -MM $< $(CFLAGS) $(ADV_CFLAGS) > $@.tmp
	@sed 's,$*\.o,$(BUILD_OBJ_PATH)/adv/$*.c.o $@,g' < $@.tmp > $@
	@rm -f $@.tmp


# z80emu stuff
$(Z80EMU_USER_NOUSE):
	mv $(Z80EMU_USER_EX) $(Z80EMU_USER_NOUSE)

$(BUILD_OBJ_PATH)/z80emu/%.c.o: $(Z80EMU_PATH)/%.c $(Z80EMU_USER_NOUSE)
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) $(Z80EMU_CFLAGS)

$(BUILD_DEP_PATH)/z80emu/%.c.d: $(Z80EMU_PATH)/%.c $(Z80EMU_USER_NOUSE)
	@mkdir -p $(dir $@)
	@rm -f $@
	@$(CC) -MM $< $(CFLAGS) $(Z80EMU_CFLAGS) > $@.tmp
	@sed 's,$*\.o,$(BUILD_OBJ_PATH)/z80emu/$*.c.o $@,g' < $@.tmp > $@
	@rm -f $@.tmp


# test gtk4 windowing stuff
$(WIN_TARGET): $(WIN_OBJS)
	@mkdir -p $(dir $@)
	$(LD) -o $@ $(WIN_OBJS) $(LFLAGS) $(WIN_LFLAGS)

$(BUILD_OBJ_PATH)/win/%.c.o: $(WIN_PATH)/%.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< $(CFLAGS) $(WIN_CFLAGS)

$(BUILD_DEP_PATH)/win/%.c.d: $(WIN_PATH)/%.c
	@mkdir -p $(dir $@)
	@rm -f $@
	@$(CC) -MM $< $(CFLAGS) $(WIN_CFLAGS) > $@.tmp
	@sed 's,$*\.o,$(BUILD_OBJ_PATH)/win/$*.c.o $@,g' < $@.tmp > $@
	@rm -f $@.tmp



include $(ADV_DEPS)
include $(Z80EMU_DEPS)
include $(WIN_DEPS)
# vim: noet
# end of file
