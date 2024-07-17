/*XM23p Loader main loop*/
/*ECED 3403   JH 2024   */

#include "XM23p.h"

// Arrays representing data and instruction memory
memory memBlock[2];
//execution clock
int clock;
char sourceFileName[MAXRECORDLENG]; //reusing MAXRECORDLENG here
//contains registers 0-7 and constants 
wordContent regFile[REGCON][REGFILE] = {{{0}, {0}, {0}, {0}, {0}, {0}, {0}, {0}},
                                        {{0}, {1}, {2}, {4}, {8}, {16}, {32}, {-1}}};
/*Program status word
 * C Z N SLP V 
*/
wordContent psw;
//IMAR, ICTRL, IMDR
wordContent instructionRegisters[IFILE];
//DMAR, DCTRL, DMBR
wordContent dataRegisters[DFILE];
//IR
code ir;
//Breakpoint address
unsigned int breakAddr;
//debug mode
unsigned char debugFlag;
//CTRL C flag
volatile sig_atomic_t ctrl_c_fnd;

//Main control loop only responsible
//for calling other functions
int main(int argc, char *argv[])
{
    //intialize ^C handler
    ctrl_c_fnd = FALSE;
    signal(SIGINT, sigint_hdlr);

    //XME File to open
    FILE *xmeFile;
    int stepExecuteFlag = 0;
    initializePipelines();
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
            case 'e':
                if(debugFlag)
                    printf("Clock   PC      Instruction     Fetch       Decode      Execute\n");
                execute();
                getchar();
                printf("Clock: %06d Instruction: %04x", clock, ir.value);
                break;
            case 's':
                displayPSW();
                getchar();
                break;
            case 'g':
                breakAddr = regFile[0][7].word;
                if (!stepExecuteFlag) breakAddr+=2;
                stepExecuteFlag = 1;
                execute();
                getchar();
                break;
            case 'f':
                printf("Debug mode toggled ");
                if(debugFlag) {
                    debugFlag = 0; 
                    printf("off\n");
                }
                else {
                    debugFlag = 1; 
                    printf("on\n");
                }
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