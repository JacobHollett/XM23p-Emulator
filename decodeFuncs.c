/*XM23p partial emulator function definitions*/
/*ECED 3403                        JH 2024   */

#include "XM23p.h"

#define upperbyte 0xFF00
#define upthree 0xE000
#define upsix 0xFC00
#define uptwo 0x1800
#define midtwo 0x0300
#define rc 0x80
#define wb 0x40
#define sc 0x38
#define D 0x07
#define B 0x07F8

char instructionSet[][4][6] = {

                {"ADD",
                "ADDC",
                "SUB",
                "SUBC"},

                {"DADD",
                "CMP",
                "XOR",
                "AND"},

                {"OR",
                "BIT",
                "BIC",
                "BIS"},

                {"MOV",
                "SWAP",
                "",
                ""},

                {"SRA",
                "RRC",
                "SWPB",
                "SXT"},

                {"MOVL",
                "MOVLZ",
                "MOVLS",
                "MOVH"}
};

//applies instructionset table to decode instructions
void decodeInstructions(){

    //Store the first instruction in a buffer
    short iBuffer = memBlock[instruction].words[regFile[0][7].word/2];
    //indicates we've found the instruction
    int flag = 0;


    while(iBuffer && regFile[0][7].word <= breakAddr){
        //search instruction set based on upper 3 bits
        if(((iBuffer & upthree)>>13) == 0x03){
            printInstruction(regFile[0][7].word, 5, ((iBuffer & uptwo)>>11), iBuffer);
            flag = 1;
        }
    
        if(!flag){
            //Search instruction set based on upper 6 bits
            switch(((iBuffer & upsix)>>10)){

                case 0x10:
                    printInstruction(regFile[0][7].word, 0, ((iBuffer & midtwo)>>8), iBuffer);
                    flag = 1;
                    break;
                case 0x11:
                    printInstruction(regFile[0][7].word, 1, ((iBuffer & midtwo)>>8), iBuffer);
                    flag = 1;
                    break;
                case 0x12:
                    printInstruction(regFile[0][7].word, 2, ((iBuffer & midtwo)>>8), iBuffer);
                    flag = 1;
                    break;
                default:
                    break;
            }
        }

        if(!flag && ((iBuffer & upperbyte)>>8) == 0x004c){

            printInstruction(regFile[0][7].word, 3, ((iBuffer & 0x0080)>>7), iBuffer);
            flag = 1;
        }

        if(!flag && ((iBuffer & upperbyte)>>8) == 0x004d){
            switch((iBuffer & 0x0038)>>3){

                case 0x00:
                    printInstruction(regFile[0][7].word, 4, ((iBuffer & 0x0038)>>3), iBuffer);
                    break;
                case 0x01:
                    printInstruction(regFile[0][7].word, 4, ((iBuffer & 0x0038)>>3), iBuffer);
                    break;
                case 0x03:
                    printInstruction(regFile[0][7].word, 4, (0x02), iBuffer);
                    break;
                case 0x04:
                    printInstruction(regFile[0][7].word, 4, (0x04), iBuffer);
                    break;
                default:
                    break;
            }
            flag = 1;
        }
        
        if(!flag)
            printf("%04x: 0x%04x\n", regFile[0][7].word, memBlock[instruction].words[regFile[0][7].word/2]);
            
        flag = 0;
        regFile[0][7].word+=2;
        iBuffer = memBlock[instruction].words[regFile[0][7].word/2];
    }
}

//prints decoded instruction to screen
void printInstruction(int address, int set, int index, int struction){

    printf("%04x: %s ", address, instructionSet[set][index]);

    if(set <= 2){
        printf("RC: %i WB: %i ", ((struction & rc)>>7), ((struction & wb)>>6));
        if(!((struction & rc)>>7))
            printf("SRC: R%i ", ((struction & sc)>>3));
        else
            printf("CON: %04x ", ((struction & sc)>>3));
        printf("DST: R%i\n", ((struction & D)>>0));
    }
    else if(set == 3){
        printf("WB: %i ", ((struction & wb)>>6));
        printf("SRC: R%i ", ((struction & sc)>>3));
        printf("DST: R%i\n", ((struction & D)>>0));
    }
    else if(set == 4){
        printf("WB: %i ", ((struction & wb)>>6));
        printf("DST: R%i\n", ((struction & D)>>0));
    }
    else if(set == 5)
        printf("B: %04x DST: R%i\n", ((struction & B)>>3), ((struction & D)>>0));

}