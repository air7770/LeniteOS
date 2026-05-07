# компилятор
CC = gcc
LD = ld

# флаги
CFLAGS = -m32 -ffreestanding -c
LDFLAGS = -m elf_i386 -T linker.ld

# файлы
SRC = main.c
OBJ = kernel.o
BIN = kernel.bin
ISO = test.iso

all: $(ISO)

# компиляция
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)

# линковка
$(BIN): $(OBJ)
	$(LD) $(LDFLAGS) $(OBJ) -o $(BIN)

# создание ISO
$(ISO): $(BIN)
	mkdir -p isodir/boot/grub
	cp $(BIN) isodir/boot/
	cp boot/grub/grub.cfg isodir/boot/grub/
	grub-mkrescue -o $(ISO) isodir

# запуск
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

# очистка
clean:
	rm -rf *.o *.bin *.iso isodir
