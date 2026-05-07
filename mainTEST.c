#include <stddef.h>
#define VMEM 0xB8000
#define WIGTH 80
#define HEIGHT 25

// это для граб что бы он видео ядро
__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002,
    0x0,
    -(0x1BADB002)
};

char* VIDEO = (char*) VMEM;
int cursor_x = 0;
int cursor_y = 0;

void put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        return;
    }

    int index = (cursor_y * WIGTH + cursor_x) * 2;

    VIDEO[index] = c;
    VIDEO[index + 1] = 0x07;

    cursor_x++;

    if (cursor_x >= WIGTH) {
        cursor_x = 0;
        cursor_y++;
    }
}



void write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
	put_char(str[i]);
    }
}

void clear_scr() {
    for (int i = 0; i < WIGTH * HEIGHT * 2; i += 2) {
	VIDEO[i] = ' ';
 	VIDEO[i + 1] = 0x07;
    }
}



void kernel_main_os() {

    clear_scr();
    write("HELLO, THIS IS MY 1 REAL KERNEL!!!");
    

    while (1);

}
