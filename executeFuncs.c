/*XM23p emulator executions definitions*/
/*ECED 3403              JH 2024   */

#include "XM23p.h"


void execute(){
    clock = 0;

    while(regFile[0][7].word <= breakAddr){
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
    printf("Clock: %i PC: %x\n", clock, regFile[0][7].word);
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

//decode stage manipulates contents of IR
//turns opcode into instruction index
//leaves other info intact
void d0(){

    if(ir.set1.opcode < 0x48)
        ir.set1.opcode = ir.set01.index;
    else if(ir.set1.opcode < 0x4c)
        ir.set1.opcode = ir.set01.index+0x8;
    else if(ir.set1.opcode == 0x4c)
        ir.set1.opcode = ir.set01.index+0xc;
    else if(ir.set1.opcode == 0x4d)
        ir.set1.opcode = ir.set01.index+0xe;
    else if(ir.set01.upopcode >= 0xc){
        ir.set01.upopcode = ir.set01.index+0x7;
    }
    else
        printf("%04x: %04x Invalid command\n", regFile[0][7].word, ir.set1.opcode);
}

void e0(){
    printf("%i\n", ir.set1.opcode);
}