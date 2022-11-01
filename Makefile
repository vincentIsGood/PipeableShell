COMPILE_OPT := -fsanitize=address -ggdb
COMPILE_CMD := gcc -Ilibs/ -Isrc/ $(COMPILE_OPT)

csrc := $(shell find src libs -type f -name '*.c')
obj  := $(csrc:%.c=build/%.o)

all: main

main: $(obj)
	$(COMPILE_CMD) -o bin/pshell $^

build/%.o: %.c
	mkdir -p $(dir $@)
	$(COMPILE_CMD) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf build/