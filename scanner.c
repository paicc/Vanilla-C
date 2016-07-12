#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>
#include "global.h"

enum ScannerState {
    S_START,  S_IDENTIFIER ,S_SLASH
};

//static ScannerState CURR_STATE;
int Line_num = 0;

int Scanner()
{
    FILE *comFile , *ScaFile;
    char Scanfile_name[16]="scanner.txt";

    comFile = fopen(Comfile_name, "r");
    ScaFile = fopen(Scanfile_name, "w");

    if(!comFile)
    {
        printf("open fail\n");
        return 0;
    }
    if(!ScaFile)
    {
        printf("open fail\n");
        return 0;
    }
    Reader(comFile , ScaFile);

    fclose(comFile);
    fclose(ScaFile);

    return 1;
}

Reader( FILE *comFile , FILE *ScaFile )
{
    char ReadBuffer[512];
    char TokenBuffer[1];
    char IdentiferBuffer[64];
    char StringBuffer[64];
    char DigitBuffer[64];
    char MathSymbolBuffer[2];
    char NumberBuffer[1];
    int ScannerState = S_START;
    memset( ReadBuffer, 0, strlen(ReadBuffer) );


    while(fgets(ReadBuffer,512,comFile) != NULL)
    {
        int NOT_Iden_Flag = 0
            ,Str_Flag = 0
            ,ReadBuffer_num = 0
            ,Identifer_num = 0
            ,String_num = 0
            ,Digit_num = 0;

        Line_num++;
        //if empty
        while (ReadBuffer[ReadBuffer_num] == ' ' || ReadBuffer[ReadBuffer_num] == '\t')
            ReadBuffer_num++;
        // 如果為單斜線，判斷是否為除號還是註解。註解的話更改狀態為S_SLASH
        if (ReadBuffer[ReadBuffer_num] == '/')
        {

            if (ReadBuffer[ReadBuffer_num +1] == '/')
                ReadBuffer[ReadBuffer_num] = '\0';

            else if (ReadBuffer[ReadBuffer_num +1] == '*')
            {
                ScannerState = S_SLASH;
                while(ReadBuffer[ReadBuffer_num] != '\0')
                {
                    ReadBuffer_num++;
                    if (ScannerState == S_SLASH && ReadBuffer[ReadBuffer_num] == '*' && ReadBuffer[ReadBuffer_num +1] == '/')
                    {
                        ScannerState = S_START;
                        ReadBuffer[ReadBuffer_num] = '\0';
                    }
                }
            }

            /*else
            {
                fprintf(ScaFile,"%d\tSPECIAL_SYMBOL\t%c\n",Line_num,ReadBuffer[ReadBuffer_num]);
                ReadBuffer_num;
            }*/

        }
        //如果為星號+單斜線，註解結束，狀態改為S_START
        if (ScannerState == S_SLASH && ReadBuffer[ReadBuffer_num] == '*' && ReadBuffer[ReadBuffer_num +1] == '/')
        {
            ScannerState = S_START;
            ReadBuffer[ReadBuffer_num] = '\0';
        }

        while(ScannerState == S_START && ReadBuffer[ReadBuffer_num] != '\0')
        {
            //判斷是否為英文
            if (isalpha (ReadBuffer[ReadBuffer_num]))
            {
                ScannerState = S_IDENTIFIER;
                memset( IdentiferBuffer, '\0' , strlen(IdentiferBuffer) );
                Identifer_num = 0;
                while (1)
                {
                    IdentiferBuffer[Identifer_num] = ReadBuffer[ReadBuffer_num];
                    //NumberBuffer[0] = ReadBuffer[ReadBuffer_num];
                    IdentiferBuffer[Identifer_num +1] = '\0';

                    //不是字母及數字
                    if (isalpha (IdentiferBuffer[Identifer_num]) == 0 && isdigit (IdentiferBuffer[Identifer_num]) == 0)
                    {
                        IdentiferBuffer[Identifer_num] ='\0';
                        if (ReadBuffer[ReadBuffer_num] !=' ')
                            ReadBuffer_num--;

                        /*if (strcmp(IdentiferBuffer,"False") == 0 || strcmp(IdentiferBuffer,"TRUE") == 0)
                            fprintf(ScaFile,"%d\tBOOL_VAL\t%s\n",Line_num,IdentiferBuffer);*/
                        if (Check_RESERVED_WORD(IdentiferBuffer) == 1)
                            fprintf(ScaFile,"%d\tRESERVED_WORD\t%s\n",Line_num,IdentiferBuffer);
                        else if (strcmp(IdentiferBuffer,"False") == 0 || strcmp(IdentiferBuffer,"TRUE") == 0)
                            fprintf(ScaFile,"%d\tBOOL_VAL\t%s\n",Line_num,IdentiferBuffer);
                        else
                            fprintf(ScaFile,"%d\tID\t%s\n",Line_num,IdentiferBuffer);

                        ScannerState =S_START;
                        break;
                    }

                    else
                    {
                        Identifer_num++;
                        ReadBuffer_num++;
                    }

                }


            }
            //非英文
            else
            {
                TokenBuffer[0] = ReadBuffer[ReadBuffer_num];
                MathSymbolBuffer[0] = ReadBuffer[ReadBuffer_num];
                MathSymbolBuffer[1] = ReadBuffer[ReadBuffer_num +1];

                Check_Token(TokenBuffer , ScaFile);
                if (Check_MATH_SYMBOL(MathSymbolBuffer ,ScaFile) == 2)
                    ReadBuffer_num++;

                if (isdigit (ReadBuffer[ReadBuffer_num]) != 0)
                {
                    Digit_num = 0;
                    while(1)
                    {
                        DigitBuffer[Digit_num] = ReadBuffer[ReadBuffer_num];

                        if (isdigit (ReadBuffer[ReadBuffer_num]) == 0)
                        {
                            ReadBuffer_num--;
                            DigitBuffer[Digit_num] = '\0';
                            fprintf(ScaFile,"%d\tDIGIT\t%s\n",Line_num,DigitBuffer);
                            break;
                        }
                        Digit_num++;
                        ReadBuffer_num++;
                    }
                }

                if (ReadBuffer[ReadBuffer_num] == '\'')
                {
                    fprintf(ScaFile,"%d\tSINGLE_QUOTES\t%c\n",Line_num,ReadBuffer[ReadBuffer_num]);
                    ReadBuffer_num++;
                    fprintf(ScaFile,"%d\tCHAR_CONST\t%c\n",Line_num,ReadBuffer[ReadBuffer_num]);
                    ReadBuffer_num++;
                    fprintf(ScaFile,"%d\tSINGLE_QUOTES\t%c\n",Line_num,ReadBuffer[ReadBuffer_num]);
                }
                if (ReadBuffer[ReadBuffer_num] == '\"')
                {
                    fprintf(ScaFile,"%d\tDOUBLE_QUOTES\t%c\n",Line_num,ReadBuffer[ReadBuffer_num]);
                    memset( StringBuffer, 0, strlen(StringBuffer) );
                    String_num =0;

                    while(1)
                    {
                        ReadBuffer_num++;
                        StringBuffer[String_num] = ReadBuffer[ReadBuffer_num];

                        if (StringBuffer[String_num] == '\"')
                        {
                            StringBuffer[String_num] = '\0';
                            fprintf(ScaFile,"%d\tSTRING_CONST\t%s\n",Line_num,StringBuffer);
                            fprintf(ScaFile,"%d\tDOUBLE_QUOTES\t%c\n",Line_num,ReadBuffer[ReadBuffer_num]);
                            break;
                        }
                        String_num++;
                    }
                }
            }

            ReadBuffer_num++;
        }
    }
}

int Check_RESERVED_WORD(char token[8] )
{
        if (strcmp(token,"bool") == 0 || strcmp(token,"char") == 0 || strcmp(token,"const") == 0 ||
            strcmp(token,"string") == 0 || strcmp(token,"int") == 0 ||strcmp(token,"if") == 0 ||
            strcmp(token,"else") == 0 || strcmp(token,"while") == 0 || strcmp(token,"read") == 0 ||
            strcmp(token,"print") == 0 || strcmp(token,"println") == 0 || strcmp(token,"main") == 0 ||strcmp(token,"const") == 0)
            return 1;
        else
            return -1;
}
Check_Token(char token[1] , FILE *ScaFile)
{
    switch (token[0])
    {
        case ':':
            fprintf(ScaFile,"%d\tCOLON_TOKEN\t%c\n",Line_num,token[0]);
            break;
        case ';':
            fprintf(ScaFile,"%d\tSEMICOLON_TOKEN\t%c\n",Line_num,token[0]);
            break;
        case '(':
            fprintf(ScaFile,"%d\tLEFTPAREN_TOKEN\t%c\n",Line_num,token[0]);
            break;
        case ')':
            fprintf(ScaFile,"%d\tRIGHTPAREN_TOKEN\t%c\n",Line_num,token[0]);
            break;
        case '{':
            fprintf(ScaFile,"%d\tLEFTBRACE_TOKEN\t%c\n",Line_num,token[0]);
            break;
        case '}':
            fprintf(ScaFile,"%d\tRIGHTBRACE_TOKEN\t%c\n",Line_num,token[0]);
            break;
        case ',':
            fprintf(ScaFile,"%d\tCOMMA_TOKEN\t%c\n",Line_num,token[0]);
            break;
        case '+':
            fprintf(ScaFile,"%d\tMATH_SYMBOL\t%c\n",Line_num,token[0]);
            break;
        case '-':
            fprintf(ScaFile,"%d\tMATH_SYMBOL\t%c\n",Line_num,token[0]);
            break;
        case '*':
            fprintf(ScaFile,"%d\tMATH_SYMBOL\t%c\n",Line_num,token[0]);
            break;
        case '/':
            fprintf(ScaFile,"%d\tMATH_SYMBOL\t%c\n",Line_num,token[0]);
            break;
        default:
            break;
    }

}

int CHECK_DIGIT (char token[2])
{
    if (strcmp(token,"0") == 0 || strcmp(token,"1") == 0 || strcmp(token,"2") == 0 ||
        strcmp(token,"3") == 0 || strcmp(token,"4") == 0 || strcmp(token,"5") == 0 ||
        strcmp(token,"6") == 0 || strcmp(token,"7") == 0 || strcmp(token,"8") == 0 ||
        strcmp(token,"9") == 0)
        return 3;
    else
        return -1;
}
int Check_MATH_SYMBOL(char token[2] ,FILE *ScaFile)
{
    if (token[0] == '<' && token[1] == '=')
    {
        fprintf(ScaFile,"%d\tMATH_SYMBOL\t<=\n",Line_num);
        return 2;
    }
    else if (token[0] == '<' && token[1] != '=')
    {
        fprintf(ScaFile,"%d\tMATH_SYMBOL\t<\n",Line_num);
        return 1;
    }
    else if (token[0] == '>' && token[1] == '=')
    {
        fprintf(ScaFile,"%d\tMATH_SYMBOL\t>=\n",Line_num);
        return 2;
    }
    else if (token[0] == '>' && token[1] != '=')
    {
        fprintf(ScaFile,"%d\tMATH_SYMBOL\t>\n",Line_num);
        return 1;
    }
    else if (token[0] == '=' && token[1] == '=')
    {
        fprintf(ScaFile,"%d\tMATH_SYMBOL\t==\n",Line_num);
        return 2;
    }
    else if (token[0] == '=' && token[1] != '=')
    {
        fprintf(ScaFile,"%d\tMATH_SYMBOL\t=\n",Line_num);
        return 1;
    }
    else if (token[0] == '!' && token[1] == '=')
    {
        fprintf(ScaFile,"%d\tMATH_SYMBOL\t!=\n",Line_num);
        return 1;
    }
    else
        return 3;

}
