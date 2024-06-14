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
    FILE *xmeFile;

    printHeader();

    for(;;){
        printf("\nCommand: ");
        switch(tolower(getchar())){
            case 'l':
                xmeFile = openFile();
                readRecords(xmeFile);
                getchar();
                break;
            case 'd':
                displayMem();
                getchar();
                break;
            case 'i':
                decodeInstructions();
                getchar();
                break;
            case 'r':
                displayRegisters();
                getchar();
                break;
            case 'c':
                changeRegister();
                getchar();
                break;
            case 't':
                changeMEM();
                getchar();
                break;
            case 'b':
                setBreak();
                getchar();
                break;
            case 'x':
                return 1;
                break;
            case '?':
                printHeader();
                getchar();
                break;
            default:
                printf("Not a valid command.\n");
                getchar();
                break;
    }
}

}