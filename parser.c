#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>
#include <string.h>
#include "global.h"
#include <unistd.h>

#define STACK_SIZE 50
#define TOKEN_SIZE 32

FILE *ScaFile
     ,*ErrFile
     ,*Obj_file;
char Current_token[64]
     ,StringBuffer[128];
int SymbolTable_num =0
    ,String_num =1;

struct Token_struct{
  char line[4];
  char type[32];
  char content[128];
}Token;

typedef struct symbol
{
    char IdentifierName[80];
    char Datatype[24];
    char Value[512];
    int Size;
    int IDType;
    int Dataobj;
}Symbol;

Symbol Symbol_Table[128];

char op[5] = {'(','+','-','*','/'};
int op_priority[5] = {0,1,1,2,2};
char stack[STACK_SIZE];
int top= -1;

int Parser(char Scan_name[128])
{

    ScaFile = fopen(Scan_name, "r");
    if(!ScaFile)
    {
        printf("Scanner.txt open fail");
        return 0;
    }

    ErrFile = fopen("Error_File.txt", "w");
    Program();

    fclose(ScaFile);
    fclose(ErrFile);
}

Get_token()
{
    char ReadBuffer[128];
    int Read_num=0
        ,Type_num=0
        ,Content_num=0
        ,Line_num=0;

    if (fgets(ReadBuffer,128,ScaFile) != NULL)
    {
        while(1)
        {
            Token.line[Line_num] = ReadBuffer[Read_num];
            if(ReadBuffer[Read_num] == '\t')
            {
                Token.line[Line_num ] = '\0';
                Read_num++;
                break;
            }
            Line_num++;
            Read_num++;
        }
        while(1)
        {
            Token.type[Type_num] = ReadBuffer[Read_num];
            if(ReadBuffer[Read_num] == '\t')
            {
                Token.type[Type_num] = '\0';
                Read_num++;
                break;
            }
            Type_num++;
            Read_num++;
        }

        while(1)
        {
            Token.content[Content_num] = ReadBuffer[Read_num];
            if(ReadBuffer[Read_num] == '\0')
            {
                Token.content[Content_num -1] = '\0';
                break;
            }
            Content_num++;
            Read_num++;
        }
        strcpy(Current_token,Token.content);
    }
    else
        strcpy(Current_token,"EOF");
}

Insert_Table(char *IdentifierName, char *Datatype, char *Value, int Size, int IDType, int Dataobj)
{
    strcpy(Symbol_Table[SymbolTable_num].IdentifierName,IdentifierName);
    strcpy(Symbol_Table[SymbolTable_num].Datatype,Datatype);
    strcpy(Symbol_Table[SymbolTable_num].Value,Value);
    Symbol_Table[SymbolTable_num].Size =Size;
    Symbol_Table[SymbolTable_num].IDType =IDType;
    Symbol_Table[SymbolTable_num].Dataobj =Dataobj;
    /*printf("Symbol_Table[%d]:IdentifierName=%s Datatype=%s Value=%s Size=%d\n",SymbolTable_num,Symbol_Table[SymbolTable_num].IdentifierName,Symbol_Table[SymbolTable_num].Datatype
           ,Symbol_Table[SymbolTable_num].Value, Symbol_Table[SymbolTable_num].Size);*/
    SymbolTable_num++;
}

VC_err (char *err)
{
    printf("Error on line:%s\t:%s\n",Token.line,err);
	fprintf(ErrFile,"Error on line:%s\t:%s\n",Token.line,err);
	exit(1);
}


Program()
{
    Get_token();
    //Current_token = "main";
    if(strcmp(Current_token,"main") == 0)
        Prog_hdr(Current_token);
    else
        VC_err("You need have \"main\" function");

    Get_token();
    if (strcmp(Current_token,"{") != 0)
        VC_err("You forget add LEFTPAREN");
    Get_token();
    Prog_body();
}

Prog_hdr(char token[16])
{
    if (strcmp(Current_token,"main") != 0)
        VC_err("You need have \"main\" function");
    Get_token();
    if (strcmp(Current_token,"(") != 0)
        VC_err("You forget add LEFTPAREN");
    Get_token();
    if (strcmp(Current_token,")") != 0)
        VC_err("You forget add RIGHTPAREN");
    Code("prog_hdr","","");
}

Prog_body()
{
    if (strcmp(Current_token,"EOF") != 0)
        DCL_List();
    if (strcmp(Current_token,"}") != 0)
    {
        STMT_List();
    }
    Code("prog_end","","");
}

STMT_List()
{
    while (1)
    {
        if (strcmp(Current_token,"}") == 0)
            break;
        Statement();
        if (strcmp(Current_token,";") == 0)
            Get_token();
    }
}

Statement()
{

    if (strcmp(Token.type, "ID") == 0)
        ASSIGNMENT();
    else if (strcmp(Current_token, "read") == 0)
        READ();
    else if (strcmp(Current_token, "print") == 0)
        PRINT();
    else if (strcmp(Current_token, "println") == 0)
        PRINTLN();
}

ASSIGNMENT()
{
    char op1[64], op2[64], oper[8], op[10] ,y ,z;
    char infix[50], postfix[50];
    int i=0 ,j=0, k=0 ,op_num=0;

    if (strcmp(Token.type, "ID") != 0)
        VC_err("process error");
    //Push_oper(Current_token);
    Get_token();
    if (strcmp(Current_token, "=") != 0)
        VC_err("code error");
    //Push_operator(Current_token)
    while(1)
    {
        Get_token();
        if (strcmp(Current_token, ";") == 0)
            break;
        else
        {
            infix[i]=Current_token[0];
            i++;
        }
    }

    to_postfix(infix,postfix);
    printf("infix : %s \tposfix : %s\n",infix , postfix);

    while(1)
    {
        if (isalpha (postfix[k]))
        {
            op[op_num] = postfix[k];
            op_num++; k++;
        }
        else
        {
            oper[0]=postfix[k];
            Code(oper, op[op_num], op[op_num-1]);
            op_num = op_num-2;
            k++;
        }
        if (postfix[k] == 0)
            break;
    }
    //Code(oper, op1, op2);
}

to_postfix (char infix[50], char postfix[50])
{
    char stack[STACK_SIZE] = {'\0'};
    int i, j, top;
    for(i = 0, j = 0, top = 0; infix[i] != '\0'; i++) switch(infix[i]) {
        case '(':              // 運算子堆疊
            stack[++top] = infix[i];
            break;
        case '+': case '-': case '*': case '/':
            while(priority(stack[top]) >= priority(infix[i])) {
                postfix[j++] = stack[top--];
            }
            stack[++top] = infix[i]; // 存入堆疊
            break;
        case ')':
            while(stack[top] != '(') { // 遇 ) 輸出至 (
                postfix[j++] = stack[top--];
            }
            top--;  // 不輸出 (
            break;
        default:  // 運算元直接輸出
            postfix[j++] = infix[i];
    }
    while(top > 0) {
        postfix[j++] = stack[top--];
    }
    postfix[j++] ='\0';
}

int priority(char *c)
{
    char op[2];
    op[0] = c;
    switch(op[0]) {
        case '+': case '-': return 1;
        case '*': case '/': return 2;
        default:            return 0;
    }
}

Push_oper(char *d1)
{
    if (top >= STACK_SIZE-1)
        VC_err("Stack full");
    top++;
    stack[top] = d1;
}


int IsEmpty()
{
	return (top < 0) ? 0 : 1;
}

READ()
{
    char cha[4];
    int num;
    if (strcmp(Current_token, "read") != 0)
        VC_err("process error");

    Get_token();
    if (strcmp(Current_token, "(") != 0)
        VC_err("you forget add \"(\"");

    Get_token();
    for (num=0; num<=SymbolTable_num; num++)
    {
        if (strcmp(Symbol_Table[num].IdentifierName ,Current_token) == 0)
            if (Symbol_Table[num].IDType == 3)
                Read_string_buffer(Symbol_Table[num].Value);
    }
    if (strcmp(Token.type, "ID") == 0)
        strcpy(cha,Current_token);

    Get_token();
    if (strcmp(Current_token, ")") != 0)
        VC_err("you forget add \")\"");
    Get_token();
    if (strcmp(cha, "\0") != 0)
        Code("read", cha, "");
}

PRINT()
{
    char str[256]
         ,string_buf[10];

    if (strcmp(Current_token, "print") != 0)
        VC_err("process error");
    Get_token();
    if (strcmp(Current_token, "(") != 0)
        VC_err("you forget add \"(\"");

    Get_token();
    if (strcmp(Current_token, "\"") != 0)
        VC_err("you forget add \"\"\"");

    Get_token();//string
    sprintf(string_buf, "_S%04d", String_num);
    Insert_Table(string_buf, "DB", Current_token, 1, 3, 3);
    String_num++;

    Get_token();
    if (strcmp(Current_token, "\"") != 0)
        VC_err("you forget add \"\"\"");

    Get_token();
    if (strcmp(Current_token, ")") != 0)
        VC_err("you forget add \")\"");
    Get_token();
    Code("print", string_buf, "");
}

PRINTLN()
{
    int num;
    char id_value[4];

    if (strcmp(Current_token, "println") != 0)
        VC_err("process error");

    Get_token();
    if (strcmp(Current_token, "(") != 0)
        VC_err("you forget add \"(\"");

    Get_token();
    if (strcmp(Token.type, "ID") == 0)
    {
        for (num=0; num<SymbolTable_num; num++)
        {
            if (strcmp(Symbol_Table[num].IdentifierName ,Current_token) ==0)
                break;
        }
        strcpy(id_value, Current_token);
        Get_token();
    }
    else
        VC_err("type error");

    if (strcmp(Current_token, ")") != 0)
        VC_err("you forget add \")\"");
    Get_token();
    Code("println", id_value, "");
}

DCL_List()
{
    while(1)
    {
        if (strcmp(Current_token,"const") == 0 || strcmp(Current_token,"int") == 0 || strcmp(Current_token,"char") == 0 ||
        strcmp(Current_token,"string") == 0 || strcmp(Current_token,"bool") == 0)
        {
            if (strcmp(Current_token,"const") == 0)
                CONST_DCL();
            else if (strcmp(Current_token,"int") == 0)
                Var_DCL(1);
            else if (strcmp(Current_token,"char") == 0)
                Var_DCL(2);
            else if (strcmp(Current_token,"string") == 0)
                Var_DCL(3);
            else
                Var_DCL(4);
        }
        else
            break;

        if (strcmp(Current_token,";") != 0)
            VC_err("; is missing");
        else
            Get_token();
    }
}

Var_DCL(int RESERVED_WORD)
{
    char name[64];
    int num
        ,String_tag=0;

    while(1)
    {
        Get_token();
        if (strcmp(Token.type,"ID") != 0)
            VC_err("No ID");
        else
        {
            //check multi Identifier
            for (num=0; num<=SymbolTable_num; num++)
            {
                if (strcmp(Symbol_Table[num].IdentifierName ,Current_token) == 0)
                    VC_err(" multi Identifier");
            }

            switch (RESERVED_WORD)
            {
            case 1:
                Insert_Table(Current_token, "DD", "0", 0, 1, 0);
                break;
            case 2:
                Insert_Table(Current_token, "DB", "0", 0, 2, 0);
                break;
            case 3:
                Insert_Table(Current_token, "DD", "_S0000", 0, 3, 3);
                if (String_tag == 0)
                {
                    Insert_Table("_S0000", "DB", "\0", 0, 0, 3);
                    String_tag=1;
                }
                break;
            case 4:
                Insert_Table(Current_token, "DB", "0", 0, 4, 0);
                break;
            default:
                VC_err("have bug");
            }

            Get_token();
            if (strcmp(Current_token ,";") == 0)
                break;
        }
    }
}

CONST_DCL()
{
    char name[64]
         ,string_buf[10]
         ,value_buf[24]
         ,char_buf[2];
    int num,type_num;

    while(1)
    {
        Get_token();
        if (strcmp(Token.type,"ID") != 0)
            VC_err("No ID");
        else
        {
            for (num=0; num<=SymbolTable_num; num++)
            {
                if (strcmp(Symbol_Table[num].IdentifierName ,Current_token) == 0)
                    VC_err(" multi Identifier");
            }
            strcpy(name,Current_token);
            Get_token();
            if (strcmp(Current_token ,"=") == 0)
            {
                Get_token();
                if (strcmp(Token.type,"DIGIT")== 0)
                    Insert_Table(name, "DD", Current_token, 1, 1, 0);
                else if (strcmp(Token.type,"SINGLE_QUOTES")== 0)
                {
                    Get_token();
                    if (strcmp(Token.type,"CHAR_CONST")!= 0)
                        VC_err("type error");
                    else
                    {
                        sprintf(char_buf, "\'%s\'", Current_token);
                        Insert_Table(name, "DB", char_buf, 1, 2, 0);
                    }
                    Get_token();
                }
                else if (strcmp(Token.type,"DOUBLE_QUOTES")== 0)
                {
                    Get_token();
                    if (strcmp(Token.type,"STRING_CONST")!= 0)
                        VC_err("type error");
                    else
                    {
                        sprintf(string_buf, "_S%04d", String_num);
                        Insert_Table(name, "DD", string_buf, 1, 3, 3);
                        Insert_Table(string_buf, "DB", Current_token, 1, 0, 3);
                        String_num++;
                    }
                    Get_token();
                }
                else if (strcmp(Token.type,"BOOL_VAL")== 0)
                {
                    if (strcmp(Current_token,"TRUE")== 0)
                        Insert_Table(name, "DB", "1", 1, 4, 0);
                    else if (strcmp(Current_token,"FALSE")== 0)
                        Insert_Table(name, "DB", "0", 1, 4, 0);
                    else
                        VC_err("You just insert \"TRUE\" or \"FALSE\"");
                }
                else if (strcmp(Token.type,"ID")== 0)
                {
                    num=0;
                    while(1)
                    {
                        if (strcmp(Symbol_Table[num].IdentifierName ,Current_token) == 0)
                        {
                            strcpy(value_buf, Symbol_Table[num].Value);
                            type_num = Symbol_Table[num].IDType;
                            if (Symbol_Table[num].Datatype == "BOOL_VAL" || Symbol_Table[num].Datatype == "CHAR_CONST")
                                Insert_Table(name, "DB", value_buf, 1, type_num, 0);
                            else
                                Insert_Table(name, "DD", value_buf, 1, type_num, 0);
                            break;
                        }
                        if (num == SymbolTable_num && strcmp(Symbol_Table[num].IdentifierName ,Current_token) != 0)
                            VC_err("the ID doesn't exist");
                        num++;
                    }
                }
                else
                    VC_err("error");
                Get_token();
                if (strcmp(Current_token ,";") == 0)
                    break;
            }
            else
                VC_err("this is const , you must add value");
        }

    }
}

Emit_data_segment()
{
    int i;
    fprintf(Obj_file,".DATA\n");
    fprintf(Obj_file, " _SID DB \"VCC Ver 1.0-A3\", 0\n");
    for (i=0; i<SymbolTable_num; i++)
    {
        if (Symbol_Table[i].Dataobj > 0)
            fprintf(Obj_file," %s\t%s\t\"%s\", %d\n",Symbol_Table[i].IdentifierName,Symbol_Table[i].Datatype,Symbol_Table[i].Value,0);
        else
            fprintf(Obj_file," %s\t%s\t%s\n",Symbol_Table[i].IdentifierName,Symbol_Table[i].Datatype,Symbol_Table[i].Value);
    }

}

int Find_type(char *d1)
{
    int num, type_num;
    for (num=0; num<SymbolTable_num; num++)
    {
        if (strcmp(Symbol_Table[num].IdentifierName,d1) == 0)
            type_num = Symbol_Table[num].IDType;
    }
    return type_num;
}

int Read_string_buffer(char *str_buf)
{
    char string_buf[10];

    sprintf(string_buf, "_S%04d", String_num);
    Insert_Table(string_buf, "DB", str_buf, 1, 0, 3);
    String_num++;
}

