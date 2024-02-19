OBJECTS = $(patsubst %.c,%.o,$(shell find src -name "*.c"))
CCARGS = -ffreestanding -Iinclude -Iflanterm -std=gnu11 -Wall -Wextra -Wpedantic -Werror

KERNEL := swk.efi

%.o: %.c
	aarch64-linux-gnu-gcc $(CCARGS) -c $< -o $@

%.o: %.s
	aarch64-linux-gnu-as $< -o $@

$(KERNEL): $(OBJECTS) flanterm/flanterm.o flanterm/backends/fb.o src/interrupts.o
	aarch64-linux-gnu-ld -nostdlib -Tsrc/link.ld --no-warn-rwx-segments $^ -o $@

run: $(KERNEL)
	-mv $< ./vfs/
	./run.sh

clean:
	-rm -rf $(OBJECTS) ./vfs/$(KERNEL) $(KERNEL)

build: $(KERNEL)

.PHONY: build


