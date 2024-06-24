/*XM23p emulator fetch, decode and */
/*  execution definitions          */
/*ECED 3403              JH 2024   */

#include "XM23p.h"

//Set 0/1 by decode, used by execute
unsigned char movFlag;

void execute(){

    while(regFile[0][7].word <= breakAddr || ((clock+2)%2)){
        if (!((clock+2) % 2))
        {
            f0();
            d0();
        }
        else{
            e0();
            f1();
        }
        clock++;
    }
}

void f0(){
    instructionRegisters[IMAR].word = regFile[0][7].word;
    instructionRegisters[ICTRL].word = READ;
    regFile[0][7].word+=2;
}

void f1(){
    instructionRegisters[IMBR].word = 
        memBlock[instruction].words[instructionRegisters[IMAR].word/2];
    ir.value = instructionRegisters[IMBR].word;
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
        ir.set1.opcode = ir.set01.index+0xc;
        movFlag = 0;
    }
    else if(ir.set1.opcode == 0x4d){
        ir.set1.opcode = ir.set01.index+0xe;
        movFlag = 0;
    }
    else if(ir.set01.upopcode >= 0xc){
        ir.set01.upopcode = ir.set01.index+0x7;
        movFlag = 1;
    }
    else
        printf("%04x: %04x Invalid command\n", regFile[0][7].word, ir.set1.opcode);
}

void e0(){

    char tempIndex;
    
    switch (movFlag)
    {
        case 0:
            tempIndex = ir.set1.opcode;
            break;
        case 1:
            tempIndex = ir.set01.upopcode;
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
    default:
        break;
    }

}