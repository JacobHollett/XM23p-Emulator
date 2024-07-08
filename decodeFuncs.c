/*XM23p partial emulator decode functions    */
/*ECED 3403                        JH 2024   */

#include "XM23p.h"

char instructions[][numStructions] = {

            "ADD",
            "ADDC",
            "SUB",
            "SUBC",
            "DADD",
            "CMP",
            "XOR",
            "AND",
            "OR",
            "BIT",
            "BIC",
            "BIS",
            "MOV",
            "SWAP",
            "SRA",
            "RRC",
            "",
            "SWPB",
            "SXT",
            "MOVL",
            "MOVLZ",
            "MOVLS",
            "MOVH",
            "SETC",
            "CLRC",
            "LD",
            "ST",
            "LDR",
            "STR"
};

//Loop over instructions in memory
//using various structures to decode them
void decodeInstructions(){

    //Store the first instruction in a buffer
    code iBuffer;
    iBuffer.value = memBlock[instruction].words[regFile[0][7].word/2];

    unsigned char tempByte;

    while(iBuffer.value && regFile[0][7].word <= breakAddr)
    {
        if(iBuffer.set1.opcode < GRP1)
            printInstruction(iBuffer.set01.index, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 1);

        else if(iBuffer.set1.opcode < GRP2)
            printInstruction(iBuffer.set01.index+OFFSET1, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 2);

        else if(iBuffer.set1.opcode == GRP2)
            printInstruction(iBuffer.set01.rc+MOVGRP, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 3);

        else if(iBuffer.set1.opcode == GRP3 && iBuffer.set1.rc == 0)
            printInstruction(iBuffer.set1.rc+MOVGRP, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 4);

        else if(iBuffer.set1.opcode == GRP3 && iBuffer.set1.rc == 1)
            printConCodes(iBuffer.set1.wb+OFFSET2, iBuffer);
    
        else if(iBuffer.set4.code == 3 && iBuffer.set4.upperBit == 0)
            printLdStr(iBuffer.set4.upperBit, iBuffer.set4.index, iBuffer);
    
        else if(iBuffer.set4.upperBit == 1)
            printLdStr(iBuffer.set4.upperBit, iBuffer.set4.index2, iBuffer);
    
        else if(iBuffer.set2.opcode >= MOVGRP){
            tempByte = concatByte(iBuffer.set2.b1, iBuffer.set2.b2);
            printMoves(iBuffer.set01.upopcode + OFFSET3, tempByte, iBuffer.set2.d);
        }
        else
            printf("%04x: %04x\n", regFile[0][7].word/2, iBuffer.set1.opcode);

        regFile[0][7].word+=2;
        iBuffer.value = memBlock[instruction].words[regFile[0][7].word/2];
    }
}


//prints decoded instruction to screen
void printInstruction(int index, int wb, int rc, int src, int d, int flag)
{
    printf("%04x: %-4s ", regFile[0][7].word, instructions[index]);
    printf("RC: %i WB: %i ", rc, wb);
    if ((!rc || flag == 3) && flag != 4)
        printf("SRC: R%-2i", src);
    else if(flag != 4)
        printf("CON: %-3i", src);
    else
        printf("        ");
    printf("DST: R%i\n", d);
}

//similar in function to printInstructions
void printMoves(int index, unsigned char byte, int d)
{
    printf("%04x: %-5s ", regFile[0][7].word, instructions[index]);
    printf("           ");
    printf("BYT: %02x DST: R%i\n", byte, d);

}

//Performs byte concatenation necessary to not
//introduce additional padding in instruction structure
unsigned char concatByte(unsigned char b1, unsigned char b2)
{
    return (b2 << 5 | b1);
}


void printConCodes(int index, code strction)
{
    printf("%04x: %-4s ", regFile[0][7].word, instructions[index]);
    printf("BITS: %i%i%i%i%i \n", strction.set3.V, strction.set3.SLP, 
    strction.set3.N, strction.set3.Z, strction.set3.C);    
}


char concatLdStr(code strction)
{
    char tempByte = ((strction.set4.INC) | (strction.set4.DEC<<1) |
     (strction.set4.PRPO<<2) | (strction.set4.index<<3) | (strction.set4.code<<4));
    if(tempByte >= 0x40) tempByte = (tempByte | 0x80);
    return tempByte;
}

void printLdStr(int flag, int index, code strction){

    printf("%04x: %-4s ", regFile[0][7].word, instructions[OFFSET4+flag*2+index]);

    switch(flag){
        case 0:
            printf("BITS: %i%i%i%i ", strction.set4.PRPO, 
            strction.set4.DEC, strction.set4.INC, strction.set4.WB);
            break;
        case 1:
            printf("OFF:  %04x ", concatLdStr(strction));
            break;
    }
    printf(" SRC: R%i DST: R%i\n", 
        strction.set4.S, strction.set4.D);
}