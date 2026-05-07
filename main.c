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

// Переменные
char* VIDEO = (char*) VMEM;
int cursor_x = 0;
int cursor_y = 0;
char input_buffer[MAX_BUFFER];
int index_buffer = 0;
// Сама клавиатура
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
// Считавание клавиатуры
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

int strcmp(const char* a, const char* b) {
    int i = 0;

    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}
// считавание символа
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
// Добавление символа 
void add_char(char C) {
    if(index_buffer < MAX_BUFFER - 1) {
        input_buffer[index_buffer++] = C;
    }
}
// Функция сложения всех символов
void write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
	    put_char(str[i]);
    }
}
// Очистка экрана
void clear_scr() {
    for (int i = 0; i < WIGTH * HEIGHT * 2; i += 2) {
	VIDEO[i] = ' ';
 	VIDEO[i + 1] = 0x07;
    }
}
// Функция echo
void cmd_echo() {
    write("echo: ");
    write("\n");
}
// Функция clear
void cmd_clear() {
    clear_scr();
    write("> ");
}
// Функция help
void cmd_help() {
    write("Commands:\n");
    write("help   - show commands\n");
    write("clear  - clear screen\n");
    write("echo   - repeat text\n");
}

// Ввод команды
void enter_command() {
    input_buffer[index_buffer] = '\0';
    
    write(input_buffer);
    write("\n");
    if (strcmp(input_buffer, "help")) {
        cmd_help();
    }
    else if (strcmp(input_buffer, "clear")) {
        cmd_clear();
    }
    else if (strcmp(input_buffer, "echo")) {
        cmd_echo();
    }
    else {
        write("Command not found");
    }


    index_buffer = 0;
}




// Основная функция
void kernel_main_os() {

    clear_scr();
    
    write("LeniteOS kernel is start!\n\n");

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
