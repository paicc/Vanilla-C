#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>
#include <string.h>
#include "global.h"
#include <unistd.h>


typedef struct Reg
{
    char name[4];
    char value[256];
}Registr;

Registr Register_book[6] = {
    {"EAX", NULL}, {"EBX", NULL}, {"ECX", NULL}, {"EDX", NULL}, {"ESI", NULL}, {"EDI", NULL}
};

int Reg_num;

Reg_free(int reg)
{
    strcpy(Register_book[reg].value, "\0");
}

Reg_bookeep(int reg, char *data)
{
    strcpy(Register_book[reg].value, data);
}
