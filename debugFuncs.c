/*XM23p Loader function definitions*/
/*ECED 3403              JH 2024   */

#include "XM23p.h"

void printHeader(){

    printf("\n +--------------------------XM23p Emulator--------------------------+");
    printf("\n | D - Display Memory Contents      R - Display Register Contents   |");
    printf("\n | C - Replace Register Value       T - Replace Memory Value        |");
    printf("\n | I - Decode Loaded Instructions   B - Set Breakpoint              |");
    printf("\n | L - Load XME File                E - Execute Instructions        |");
    printf("\n | ? - Display this Menu            X - Close Emulator              |");
    printf("\n +------------------------------------------------------------------+\n");
}


void displayRegisters(){

    for (int i = 0; i < REGFILE; i++){

        printf("\nR%i: 0x%04x", i, regFile[0][i].word);

    }
    printf("\n");
}


void changeRegister(){

    unsigned int reg;
    unsigned int value;

    printf("\nEnter register number and value: ");
    scanf("%x %x", &reg, &value);

    regFile[0][reg].word = value;
}


void changeMEM(){

    char memchoice[MAXRECORDLENG];
    int mem;
    unsigned int addr;
    unsigned int value;


    printf("\nDMEM or IMEM: ");
    scanf("%74s", memchoice);

    for(int i = 0; i < MAXRECORDLENG; i++){
        memchoice[i] = tolower(memchoice[i]);
    }

    if(!strcmp(memchoice, "imem")){
        mem = instruction;
    }
    else if(!strcmp(memchoice, "dmem")){
        mem = data;
    }
    else{
        printf("That's not a valid memory!\n");
        return;
    }

    printf("Address and value: ");
    scanf("%x %x", &addr, &value);

    memBlock[mem].words[addr/2] = value;

}


void setBreak(){

    printf("Break point address (Byte Address): ");
    scanf("%x", &breakAddr);
    printf("Break point set at address 0x%04x\n", breakAddr);
}