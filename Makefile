OBJECTS := $(patsubst src/%.c,out/%.o,$(shell find src -name "*.c"))
OBJECTS += $(patsubst src/%.s,out/%.o,$(shell find src -name "*.s"))
CCFLAGS := -ffreestanding -Iinclude -Iflanterm -std=gnu11 -Wall -Wextra -Wpedantic -Werror

KERNEL := swk.elf

out/%.o: src/%.c
	aarch64-linux-gnu-gcc $(CCFLAGS) -c $< -o $@

out/%.o: src/%.s
	aarch64-linux-gnu-as $< -o $@

out/flanterm/%.o: flanterm/%.c
	aarch64-linux-gnu-gcc $(CCFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS)
	aarch64-linux-gnu-ld -nostdlib -Tsrc/link.ld --no-warn-rwx-segments $^ -o $@

run: $(KERNEL)
	-mv $< ./vfs/
	./run.sh

clean:
	-rm -rf $(OBJECTS) ./vfs/$(KERNEL) $(KERNEL)

build: $(KERNEL)

.PHONY: build


