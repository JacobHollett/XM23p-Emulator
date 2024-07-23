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
            "STR",
            "BL",
            "BEQ",
            "BNE",
            "BC",
            "BNC",
            "BN",
            "BGE",
            "BLT",
            "BRA"
};

//Loop over instructions in memory
//using various structures to decode them
void decodeInstructions(){

    //Store the first instruction in a buffer
    code iBuffer;
    iBuffer.value = memBlock[instruction].words[regFile[0][PC].word/2];

    unsigned char tempByte;
    short offset;

    while(regFile[0][PC].word <= breakAddr)
    {
        if (iBuffer.set5.up3 == 0)
        {
            offset = concatBRC(iBuffer);
            printBranches(BL, offset);
        }else if(iBuffer.set5.up3 == 1){
            offset = concatBRC(iBuffer);
            printBranches(BL+iBuffer.set5.low3+1, offset);
        }
        else if(iBuffer.set1.opcode < GRP1)
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
            printf("%04x: %04x\n", regFile[0][PC].word/2, iBuffer.set1.opcode);

        regFile[0][PC].word+=2;
        iBuffer.value = memBlock[instruction].words[regFile[0][PC].word/2];
    }
}


//prints decoded instruction to screen
void printInstruction(int index, int wb, int rc, int src, int d, int flag)
{
    printf("%04x: %-4s ", regFile[0][PC].word, instructions[index]);
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
    printf("%04x: %-5s ", regFile[0][PC].word, instructions[index]);
    printf("           ");
    printf("BYT: %02x DST: R%i\n", byte, d);

}

//Performs byte concatenation necessary to not
//introduce additional padding in mov instruction structure
unsigned char concatByte(unsigned char b1, unsigned char b2)
{
    return (b2 << 5 | b1);
}

//Handles printing of set/clr commands
void printConCodes(int index, code strction)
{
    printf("%04x: %-4s ", regFile[0][PC].word, instructions[index]);
    printf("BITS: %i%i%i%i%i \n", strction.set3.V, strction.set3.SLP, 
    strction.set3.N, strction.set3.Z, strction.set3.C);    
}

//Handles offset concatenation for ld/str instructions
char concatLdStr(code strction)
{
    char tempByte = ((strction.set4.INC) | (strction.set4.DEC<<1) |
     (strction.set4.PRPO<<2) | (strction.set4.index<<3) | (strction.set4.code<<4));
    if(tempByte >= 0x40) tempByte = (tempByte | 0x80); //sign extending based on upper bit
    return tempByte;
}

//Handles debug printing of ld/str instructions
void printLdStr(int flag, int index, code strction){

    printf("%04x: %-4s ", regFile[0][PC].word, instructions[OFFSET4+flag*2+index]);

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

//Handles debug printing of branch instructions
void printBranches(int index, short offset){

    printf("%04x: %-5s", regFile[0][PC].word, instructions[index]);
    printf("OFF: %04x\n", offset);

}

//Concatenate the offset for branch instructions
short concatBRC(code strction){

    wordContent temp;
    temp.word = strction.set5.off1 + (strction.set5.off2 << 8);
    if (!strction.set5.up3){
        temp.word += (strction.set5.low3 << 10);
        //Sign extending for BL
        if(temp.bit[1].b4) temp.word = (temp.word | 0xE000);
    } else if(temp.bit[1].b1) temp.word = (temp.word | 0xFC00);
    //The above sign extends for each other instruction
    temp.word = (temp.word<<1);
    return temp.word;
}