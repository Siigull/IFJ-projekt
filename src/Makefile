EXE = app

CC = gcc
LD = gcc

CFLAGS = 
DEPFLAGS = -MMD -MP
LDFLAGS = 
LDLIBS = 

BIN = .
OBJ = .
SRC = .

ifeq ($(CLANG_FORMAT),)
CLANG_FORMAT := clang-format
endif

SOURCES := $(wildcard $(SRC)/*.c)

OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c))

DEPENDS := $(OBJECTS:.o=.d)

COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@

LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $@

.DEFAULT_GOAL = all

.PHONY: all
all: $(BIN)/$(EXE)

$(BIN)/$(EXE): $(SRC) $(OBJ) $(BIN) $(OBJECTS)
	$(LINK.o)	

$(SRC):
	mkdir -p $(SRC)

$(OBJ):
	mkdir -p $(OBJ)

$(BIN):
	mkdir -p $(BIN)

$(OBJ)/%.o:$(SRC)/%.c
	$(COMPILE.c) $<

.PHONY: run
run: $(BIN)/$(EXE)
	./$(BIN)/$(EXE)

.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDS)
	$(RM) $(BIN)/$(EXE)

.PHONY: reset
reset:
	$(RM) -r $(OBJ)
	$(RM) -r $(BIN)

.PHONY: format
format:
	@find $(SOURCE_DIR) -type f -regex '.*\.\(c\|h\)' -print0 | xargs -0 $(CLANG_FORMAT) --dry-run

.PHONY: format-fix
format-fix:
	@find $(SOURCE_DIR) -type f -regex '.*\.\(c\|h\)' -print0 | xargs -0 $(CLANG_FORMAT) -i

-include $(DEPENDS)
