/*XM23p emulator fetch, decode and */
/*  execution definitions          */
/*ECED 3403              JH 2024   */

#include "XM23p.h"

//Set by decode, used by execute
//to distinguish between arithmetic
//moves and load/stores
unsigned char movFlag;

//copy of IMBR used for printing
code oldIMBR;

//Main execution loop calling execution stages
void execute(){

    while(regFile[0][7].word <= breakAddr || ((clock+2)%2)){
        if(debugFlag) printf(" %03i", clock);
        if (!((clock+2) % 2))
        {
            if(debugFlag){
                printf("   %04x",  regFile[0][7].word);
                printf("        %04x", memBlock[instruction].words[regFile[0][7].word/2]);
            }
            e1();
            f0();
            d0();
            if(debugFlag){
                printf("       F0: %04x", instructionRegisters[MAR].word);
                printf("     D0: %04x\n", instructionRegisters[MBR].word);
            }
        }
        else{

            e0();
            f1();

            if(debugFlag){
                printf("            ");
                printf("              ");
                printf("F1: %04x", instructionRegisters[MBR].word);
                printf("            ");
                printf("     E0: %04x\n", oldIMBR.value );
            }
        }
        clock++;
    }
}

void f0(){
    instructionRegisters[MAR].word = regFile[0][7].word;
    instructionRegisters[CTRL].word = READ;
    regFile[0][7].word+=2;
}

//Hold old IMBR, fetch new one and make a
//copy to be manipulated
void f1(){
    oldIMBR.value = instructionRegisters[MBR].word;
    instructionRegisters[MBR].word = 
        memBlock[instruction].words[instructionRegisters[MAR].word/2];
    ir.value = instructionRegisters[MBR].word;
}

//decode stage copies contents of IR to
//a temp register then translates
//its opcode to an index
//Sets flag to identify move instructions
void d0(){
    
    if(ir.set1.opcode == 0){
        ir.set01.upopcode = 8;
        movFlag = 1;
    }
    else if(ir.set1.opcode < 0x48){
        ir.set1.opcode = ir.set01.index;
        movFlag = 0;
    }
    else if(ir.set1.opcode < 0x4c){
        ir.set1.opcode = ir.set01.index+0x8;
        movFlag = 0;
    }
    else if(ir.set1.opcode == 0x4c){
        ir.set1.opcode = ir.set01.rc+0xc;
        movFlag = 0;
    }
    else if(ir.set1.opcode == 0x4d && ir.set1.rc == 0){
        ir.set1.opcode = ir.set01.rc+0xc;
        movFlag = 0;
    }
    else if(ir.set1.opcode == 0x4d && ir.set1.rc == 1){
        ir.set1.opcode = ir.set01.wb+0x17;
        movFlag = 0;
    }
    else if(ir.set4.code == 3 && ir.set4.upperBit == 0){
        movFlag = 2;
    }
    else if(ir.set4.upperBit == 1){
        movFlag = 3; 
    }
    else if(ir.set01.upopcode >= 0xc){
        ir.set01.upopcode+=0x7;
        movFlag = 1;
    }
    else
        printf("%04x: %04x Invalid command\n", regFile[0][7].word, ir.set1.opcode);
}

//takes decoded instruction
//and passes them to methods to be executed
//for ld/str sets data registers
void e0(){

    unsigned char tempIndex;
    unsigned char tempByte;
    
    switch (movFlag)
    {
        case 0:
            tempIndex = ir.set1.opcode;
            break;
        case 1:
            tempIndex = ir.set01.upopcode;
            break;
        //ld/st cases
        case 2:
            tempIndex = 25;
            ldStHandle(ir.set4.index);
            break;
        //ldr/str
        case 3:
            tempIndex = 25;
            tempByte = concatLdStr(ir);
            ldrStrHandle(ir.set4.index2, tempByte);
            break;
        default:
            tempIndex = ir.set1.opcode;
            break;
    }

    switch (tempIndex)
    {
    case 0:
        ADD(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d, 0);
        break;
    case 1:
        ADD(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d, psw.bit[0].b0);
        break;
    case 2:
        SUB(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d, 0, 0);
        break;
    case 3:
        SUB(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d, psw.bit[0].b0, 1);
        break;
    case 4:
        DADD(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 5:
        CMP(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 6:
        XOR(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 7:
        AND(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 8:
        OR(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 9:
        BIT(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 10:
        BIC(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 11:
        BIS(ir.set1.rc, ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 12:
        MOV(ir.set1.wb, ir.set1.sc, ir.set1.d);
        break;
    case 13:
        SWAP(ir.set1.sc, ir.set1.d);
        break;
    case 14:
        SRA(ir.set1.wb, ir.set1.d);
        break;
    case 15:
        RRC(ir.set1.wb, ir.set1.d);
        break;
    case 17:
        SWPB(ir.set1.d);
        break;
    case 18:
        SXT(ir.set1.d);
        break;
    case 19:
        tempByte = concatByte(ir.set2.b1, ir.set2.b2);
        MOVL(ir.set2.d, tempByte);
        break;
    case 20:
        tempByte = concatByte(ir.set2.b1, ir.set2.b2);
        MOVLZ(ir.set2.d, tempByte);
        break;
    case 21:
        tempByte = concatByte(ir.set2.b1, ir.set2.b2);
        MOVLS(ir.set2.d, tempByte);
        break;
    case 22:
        tempByte = concatByte(ir.set2.b1, ir.set2.b2);
        MOVH(ir.set2.d, tempByte);
        break;
    case 23:
        SETCC(ir.set3.V, ir.set3.SLP, ir.set3.N, ir.set3.Z, ir.set3.C);
        break;
    case 24:
        CLRCC(ir.set3.V, ir.set3.SLP, ir.set3.N, ir.set3.Z, ir.set3.C);
        break;
    default:
        break;
    }
}

//Second execution step used for ld/st
void e1()
{
    if(movFlag > 1){
        switch(dataRegisters[CTRL].bytes[0]){
            case 0:
                if(!dataRegisters[CTRL].bytes[1])
                    regFile[0][oldIMBR.set4.D].word = 
                    memBlock[data].words[dataRegisters[MAR].word/2];
                else
                    regFile[0][oldIMBR.set4.D].bytes[0] = 
                    memBlock[data].bytes[dataRegisters[MAR].word];
                break;
            case 1:
                if(!dataRegisters[CTRL].bytes[1])
                    memBlock[data].words[dataRegisters[MAR].word/2] = 
                    regFile[0][oldIMBR.set4.S].word;
                else
                    memBlock[data].bytes[dataRegisters[MAR].word] = 
                    regFile[0][oldIMBR.set4.S].word;
                break;
        }
    }
}

//Handles setting up data pipeline registers
//flag handles which instruction
void ldStHandle(int flag)
{
    //used as a scaling factor to increment/decrement by bytes or words
    unsigned char quanta = 2 - ir.set4.WB;

    switch(flag){
        case 0:
            if(!ir.set4.PRPO){
                regFile[0][ir.set4.S].word += ir.set4.INC * quanta;
                regFile[0][ir.set4.S].word -= ir.set4.DEC * quanta;
            }
            dataRegisters[MAR].word = regFile[0][ir.set4.S].word;
            if(ir.set4.PRPO){
                regFile[0][ir.set4.S].word += ir.set4.INC * quanta;
                regFile[0][ir.set4.S].word -= ir.set4.DEC * quanta;
            }
            break;
        case 1:
            if(!ir.set4.PRPO){
                regFile[0][ir.set4.D].word += ir.set4.INC * quanta;
                regFile[0][ir.set4.D].word -= ir.set4.DEC * quanta;
            }
            dataRegisters[MAR].word = regFile[0][ir.set4.D].word;
            if(ir.set4.PRPO){
                regFile[0][ir.set4.D].word += ir.set4.INC * quanta;
                regFile[0][ir.set4.D].word -= ir.set4.DEC * quanta;
            }
            break;
    }
    dataRegisters[CTRL].bytes[0] = ir.set4.index;
    dataRegisters[CTRL].bytes[1] = ir.set4.WB;
}

//handles setting up data pipeline registers
//flag handles which instruction
void ldrStrHandle(int flag, unsigned char byte)
{
    switch(flag){
        case 0:
            dataRegisters[MAR].word = regFile[0][ir.set4.S].word + byte;
            break;
        case 1:
            dataRegisters[MAR].word = regFile[0][ir.set4.D].word + byte;
            break;
    }
    dataRegisters[CTRL].bytes[0] = ir.set4.index2;
    dataRegisters[CTRL].bytes[1] = ir.set4.WB;
}