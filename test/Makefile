APP := test
VER :=
# 不立即展开 $(APP) 和 $(VER)
PKG = $(APP)-$(VER)

DEBUG := False

CC := gcc
RM := rm -rf
CP := cp -r
MKDIR := mkdir -p

SRCS := $(sort $(wildcard *.c) $(filter-out %main.c, $(wildcard ../src/*.c)))
DEPS := $(SRCS:.c=.c.d)
OBJS := $(SRCS:.c=.c.o)

THIRD_LIBS := sdl2 SDL2_ttf SDL2_image

CFLAGS  := $(foreach dep,$(THIRD_LIBS),$(shell pkg-config --cflags $(dep))) \
    -std=c99 -MMD -MP -Wno-unused-result -DVERSION=\"v${VER}\"
LDFLAGS := $(foreach dep,$(THIRD_LIBS),$(shell pkg-config --libs $(dep)))

CFLAGS_DEBUG    := -g3 -O0 -DDEBUG -Wall -Wno-unused -Wno-format-truncation \
    -Wformat -fsanitize=address
LDFLAGS_DEBUG   := -fsanitize=address -rdynamic
CFLAGS_RELEASE  := -O2 -DNDEBUG -g -Wall -Wno-format-truncation -Werror -Wformat
LDFLAGS_RELEASE :=

ifeq ($(DEBUG), True)
CFLAGS  += $(CFLAGS_DEBUG)
LDFLAGS += $(LDFLAGS_DEBUG)
else
CFLAGS  += $(CFLAGS_RELEASE)
LDFLAGS += $(LDFLAGS_RELEASE)
endif

run: $(APP)
	@echo "run test case $(APP)"
	@echo "**Test report is shown below**"
	@./$(APP)

$(APP): $(OBJS)
	@$(CC) -o $@ $^ $(LDFLAGS)
	@echo "GEN $@"

%.c.o: %.c
	@$(CC) -o $@ -c $< $(CFLAGS)
	@echo "CC  $@"

-include $(DEPS)

define rm
	@$(RM) $(1)
	@echo "RM $(1)"

endef

clean:
	$(foreach f,$(sort $(OBJS) $(APP)),$(call rm,$(f)))

distclean: clean
	$(foreach f,$(sort $(DEPS)),$(call rm,$(f)))
	$(call rm,"compile_commands.json")

tar:

.PHONY: clean distclean tar
