CC := gcc
RM := rm -rf
CP := cp -r
MKDIR := mkdir -p

#编译选项在这里修改
#TARGET := LINUX
IS_DEBUG := DEBUG
#IS_DEBUG :=

PROGRAME := mine
VERSION := v0.0.1

APP := $(PROGRAME)-$(VERSION)

CFLAGS := -DVERSION=\"$(VERSION)\" -MMD -MP `sdl2-config --cflags`
LDFLAGS := `sdl2-config --libs` -lSDL2_image -lSDL2_ttf

CFLAGS_WIN := -DWINDOWS
LDFLAGS_WIN :=
CFLAGS_DEBUG := -DDEBUG -g -O0 -Wall -Wno-unused
LDFLAGS_DEBUG :=
CFLAGS_RELEASE := -O2 -W -Wall -DNDEBUG

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

ifeq ($(TARGET), WIN)
	CFLAGS += $(CFLAGS_WIN)
	LDFLAGS += $(LDFLAGS_WIN)
	APP := $(APP)-win
else
	CFLAGS += -DLINUX
	APP := $(APP)-linux
endif
ifeq ($(IS_DEBUG), DEBUG)
	CFLAGS += $(CFLAGS_DEBUG)
else	# not debug
ifeq ($(TARGET), WIN)
	LDFLAGS += -mwindows
endif	# windows && not debug
	CFLAGS += $(CFLAGS_RELEASE)
endif

all: $(PROGRAME) test

$(PROGRAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
-include $(DEPS)

TEST_APP := test-$(VERSION)
TEST_SRCS := $(wildcard test/*.c) $(filter-out %main.c, $(SRCS))
TEST_OBJS := $(TEST_SRCS:.c=.o)
TEST_DEPS := $(TEST_SRCS:.c=.d)
test: $(TEST_OBJS)
	$(CC) -o $(TEST_APP) $^ $(LDFLAGS)
	@echo "**Test report is shown below**"
	@./$(TEST_APP)
-include $(TEST_DEPS)

clean:
	$(RM) $(DEPS) $(OBJS) $(PROGRAME)
distclean: clean
	$(RM) $(APP) $(APP).zip
	$(RM) $(TEST_OBJS) $(TEST_DEPS) $(TEST_APP)

.PHONY: clean distclean test
