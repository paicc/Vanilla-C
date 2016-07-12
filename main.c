#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>
#include <time.h>
#include "global.h"

char Comfile_name[128] ;
char Scan_name[128] ="scanner.txt";

int main(int argc, char *argv[])
{
    if (argc != 2)
        printf("Input Error;\n");
    else
        strcpy(Comfile_name, argv[1]);

    vcc_prologue();
	start_up();
	translate();
	clean_up();
	vcc_epilogue();
}

vcc_prologue()
{
    time_t t;
  	struct tm *tm_p;

    //display process start-up message
    printf("Compiler Start !!!\n");

    //record data/time
    time(&t);
  	tm_p = localtime(&t);
  	printf("%d/%d/%d %d:%d:%d\n",tm_p->tm_mon+1,tm_p->tm_mday,tm_p->tm_year+1900,tm_p->tm_hour,tm_p->tm_min,tm_p->tm_sec);
}

start_up()
{
    Generator();
}

translate()
{
    char Compile_name[128];
    //strcpy(Compile_name,Comfile_name);
    //strcat(Compile_name,".vc");
    //printf("file name= %s\n",Compile_name);
    if(!Scanner()){
		printf("Create %s failed!\n",Scan_name);
		exit(1);
	}
	else{
		printf("Create a file %s\n" ,Scan_name);
	}

    Parser(Scan_name);
}

clean_up()
{

}


vcc_epilogue()
{
    time_t t;
  	struct tm *tm_p;

    //display process start-up message
    printf("Compiler Complete !!!\n");

    //record data/time
    time(&t);
  	tm_p = localtime(&t);
  	printf("%d/%d/%d %d:%d:%d\n",tm_p->tm_mon+1,tm_p->tm_mday,tm_p->tm_year+1900,tm_p->tm_hour,tm_p->tm_min,tm_p->tm_sec);
}
