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
            "SETCC",
            "CLRCC"
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
        if(iBuffer.set1.opcode < 0x48)
            printInstruction(iBuffer.set01.index, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 1);
        else if(iBuffer.set1.opcode < 0x4c)
            printInstruction(iBuffer.set01.index+0x8, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 2);
        else if(iBuffer.set1.opcode == 0x4c)
            printInstruction(iBuffer.set01.rc+0xc, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 3);
        else if(iBuffer.set1.opcode == 0x4d && iBuffer.set1.rc == 0)
            printInstruction(iBuffer.set1.rc+0xc, iBuffer.set1.wb, 
                    iBuffer.set1.rc, iBuffer.set1.sc, iBuffer.set1.d, 4);
        else if(iBuffer.set1.opcode == 0x4d && iBuffer.set1.rc == 1)
            printConCodes(iBuffer.set1.wb+0x17, iBuffer);
        else if(iBuffer.set01.upopcode >= 0xc){
            tempByte = concatByte(iBuffer.set2.b1, iBuffer.set2.b2);
            printMoves(iBuffer.set01.upopcode + 0x7, tempByte, iBuffer.set2.d);
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

unsigned char concatByte(unsigned char b1, unsigned char b2)
{
    return (b2 << 5 | b1);
}


void printConCodes(int index, code strction)
{
    printf("%04x: %-5s ", regFile[0][7].word, instructions[index]);
    printf("BITS: %i%i%i%i%i \n", strction.set3.V, strction.set3.SLP, 
    strction.set3.N, strction.set3.V, strction.set3.C);    
}