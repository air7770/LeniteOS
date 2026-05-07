#include <stddef.h>
#define VMEM 0xB8000 // video code
#define WIGTH 80
#define HEIGHT 25
#define MAX_BUFFER 128

__attribute__((section(".multiboot")))
const unsigned int multiboot_header[] = {
    0x1BADB002,
    0x0,
    -(0x1BADB002)
};

unsigned char inb(unsigned short port) {
    unsigned char result;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(result)
        : "Nd"(port)
    );

    return result;
}

// variable for os, it's var need's for video and cursor
char* VIDEO = (char*) VMEM;
int cursor_x = 0;
int cursor_y = 0;
char input_buffer[MAX_BUFFER];
int index_buffer = 0;

char keyboard_map[128] = {
    0,
    27,
    '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=',
    '\b',
    '\t',
    'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']',
    '\n',
    0,
    'a', 's', 'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';', '\'',
    '`',
    0,
    '\\',
    'z', 'x', 'c', 'v', 'b', 'n',
    'm', ',', '.', '/',
    0,
    '*',
    0,
    ' ',
};

char keyboard_gc() {
    static unsigned char last = 0;
    unsigned char scancode;

    scancode = inb(0x60);

    if (scancode == 0 || scancode == last) {
        return 0;
    }

    last = scancode;

    if (scancode & 0x80) return 0;

    if (scancode > 0 && scancode < 128) {
        return keyboard_map[scancode];
    }

    return 0;



}
/*
void put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        return;
    }
    
}
*/
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

void add_char(char C) {
    if(index_buffer < MAX_BUFFER - 1) {
        input_buffer[index_buffer++] = C;
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

void enter_command() {
    input_buffer[index_buffer] = '\0';
    
    write("\nTEST- ");
    write(input_buffer);
    write("\n");

    index_buffer = 0;
}

void kernel_main_os() {

    clear_scr();
    
    write("HELLO, THIS IS MY 1 REAL KERNEL!!!");
    

    

    while (1) {
        char ckey = keyboard_gc();
        if (!ckey) continue;


        if (ckey == '\n') {
            enter_command();
            write("> ");
        }   
        else {
            put_char(ckey);
            add_char(ckey);
        }
        
        
    }

}
