#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>
#include <string.h>
#include "global.h"
#include <time.h>

FILE    *Obj_file;

char    Obj_name[128]
        ,*input_name_tmp
        ,Output_name[128];



Generator()
{
    time_t t;
  	struct tm *tm_p;
  	time(&t);
  	tm_p = localtime(&t);

    strcpy(Output_name,Comfile_name);
    input_name_tmp = strtok(Output_name,".");
    strcpy(Output_name,input_name_tmp);
    sprintf(Obj_name,"%s.asm",Output_name);
	Obj_file = fopen(Obj_name,"w");
	fprintf(Obj_file,"Title Vanilla-C compiler generated code (%s.asm)\n",Comfile_name);
	fprintf(Obj_file,"; VCC Ver 1.0-A4 DATE: %d/%d/%d\n", tm_p->tm_mon+1,tm_p->tm_mday,tm_p->tm_year+1900);
}

Code(char Operation[64], char *d1, char *d2)
{
    if (strcmp(Operation, "prog_hdr") == 0)
        Emit_prologue();
    else if (strcmp(Operation, "prog_end") == 0)
        Emit_epilogue();
    /*else if (strcmp(Operation, "=") == 0)
        Emit_assign(*d1, *d2);
    else if (strcmp(Operation, "+") == 0)
        Emit_add(*d1, *d2);
    else if (strcmp(Operation, "-") == 0)
        Emit_sub(*d1, *d2);
    else if (strcmp(Operation, "*") == 0)
        Emit_mul(*d1, *d2);
    else if (strcmp(Operation, "/") == 0)
        Emit_div(*d1, *d2);*/
    /*else if (strcmp(Operation, "%") == 0)
        Emit_mod(*d1, *d2);
    else if (strcmp(Operation, "<=") == 0)
        Emit_sub(*d1, *d2);
    else if (strcmp(Operation, "<") == 0)
        Emit_mul(*d1, *d2);
    else if (strcmp(Operation, ">=") == 0)
        Emit_div(*d1, *d2);
    else if (strcmp(Operation, ">") == 0)
        Emit_mod(*d1, *d2);
    else if (strcmp(Operation, "==") == 0)
        Emit_mod(*d1, *d2);
    else if (strcmp(Operation, "!=") == 0)
        Emit_sub(*d1, *d2);*/
    else if (strcmp(Operation, "read") == 0)
        Emit_read(d1);
    else if (strcmp(Operation, "print") == 0)
        Emit_print(d1);
    else if (strcmp(Operation, "println") == 0)
        Emit_println(d1);
}

Emit_add(char *Opcode, char *Op1, char *Op2)
{

}

Emit_sub(char *Opcode, char *Op1, char *Op2)
{

}

Emit_mul(char *Opcode, char *Op1, char *Op2)
{

}

Emit_div(char *Opcode, char *Op1, char *Op2)
{

}

Emit(char *Opcode, char *Op1, char *Op2)
{
    fprintf(Obj_file, "\t%s", Opcode);
    if (strcmp (Op1, "\0")!= 0)
        fprintf(Obj_file, "\t%s", Op1);
    if (strcmp (Op2, "\0")!= 0)
        fprintf(Obj_file, ",%s", Op2);
    fprintf(Obj_file, "\n");
}

Emit_read(char *d1)
{
    int type_num;

    Reg_free(0);
    type_num = Find_type(d1);

    switch (type_num)
    {
    case 1:
        Emit("call", "ReadInt", "");
        Emit("mov", d1, "EAX");
        Reg_bookeep(0,d1);
        break;
    case 2:
        Emit("call", "ReadChar", "");
        Emit("mov", d1, "AL");
        Reg_bookeep(0,d1);
        break;
    case 3:
        /*no work*/
        break;
    case 4:
        Emit("mov", d1, FALSE);
        Emit("call", "ReadChar", "");
        //Emit if (AL == ¡¥T¡¦) move T to d_obj;¡¨
        break;
    }
}

Emit_print(char *d1)
{
    int type_num;
    char String[24] = "OFFSET\t";

    Reg_free(0);
    type_num = Find_type(d1);

    switch (type_num)
    {
    case 1:
        Emit("mov", "EAX", d1);
        Emit("call", "WriteInt", "");
        break;
    case 2:
        Emit("call", "ReadChar", "");
        Emit("mov", d1, "AL");
        Reg_bookeep(0,d1);
        break;
    case 3:
        strcat(String, d1);
        Emit("mov", "EDX", String);
        Emit("call", "WriteString", "");
        Reg_bookeep(0, String);
        break;
    case 4:
        Emit("mov", d1, FALSE);
        Emit("call", "ReadChar", "");
        //Emit if (AL == ¡¥T¡¦) move T to d_obj;¡¨
        break;
    default:
        break;
    }
}

Emit_println(char *d1)
{
    int type_num;

    type_num = Find_type(d1);

    switch (type_num)
    {
    case 1:
        Emit("mov", "EAX", d1);
        Emit("call", "WriteInt", "");
        Emit("call", "Crlf", "");
        break;
    case 2:
        Emit("mov", "AL", d1);
        Emit("call", "WriteChar", "");
        Emit("call", "Crlf", "");
        break;
    case 3:
        /*no work*/
        break;
    case 4:
        /*Emit("mov", d1, FALSE);
        Emit("call", "ReadChar", "");
        //Emit if (AL == ¡¥T¡¦) move T to d_obj;¡¨*/
        break;
    default:
        break;
    }
}
Emit_lable(int L)
{
    fprintf(Obj_file, "_L%4d:\n", L);
}

Emit_prologue()
{
    fprintf(Obj_file,"TITLE\t%s\n", Comfile_name);
    fprintf(Obj_file,"INCLUDE Irvine32.inc\n");
    fprintf(Obj_file,".STACK 2048 \n");
    fprintf(Obj_file,".CODE\n");
    fprintf(Obj_file," _main\tPROC\n");
}

Emit_epilogue()
{
    fprintf(Obj_file,"\texit\n");
    fprintf(Obj_file,"_main\tENDP\n");
    Emit_data_segment();
    fprintf(Obj_file,"\tEND\t_main\n");
    fclose(Obj_file);
}



Emit_comments(char comment[256])
{
    fprintf(Obj_file,";%s\n", comment);
}
