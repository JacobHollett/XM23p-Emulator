/*XM23p Loader main loop*/
/*ECED 3403   JH 2024   */

#include "XM23p.h"

// Arrays representing data and instruction memory
memory memBlock[2];

char sourceFileName[MAXRECORDLENG]; //reusing MAXRECORDLENG here

wordContent regFile[REGCON][REGFILE];

unsigned int breakAddr;

//Main directly prints diagnostic info and opens files
int main(int argc, char *argv[])
{
//XME File to open
char xmeFileName[MAXRECORDLENG]; //reusing MAXRECORDLENG here again
FILE *xmeFile;

printf("\nXM23p Loader\n");
printf("D - Display Contents of Memory      R - Display Register Data           C - Change Register Value\n");
printf("T - Replace Memory Value            I - Decode Loaded Instructions      B - Set Breakpoint  X - Exit Program\n");
printf("\nFile to load: ");
scanf("%s", xmeFileName);
printf("Opening %s...\n", xmeFileName);

xmeFile = fopen(xmeFileName, "r");

readRecords(xmeFile);

while(tolower(getchar()) != tolower('x')){
    printf("\nCommand: ");
    
    switch(tolower(getchar())){
        case 'd':
            displayMem();
            break;

        case 'i':
            decodeInstructions();
            break;

        case 'r':
            displayRegisters();
            break;

        case 'c':
            changeRegister();
            break;

        case 't':
            changeMEM();
            break;
        
        case 'b':
            setBreak();
            break;

        case 'x':
            return 1;
            break;

        default:
            printf("Not a valid command.\n");
            break;
    }
}

}