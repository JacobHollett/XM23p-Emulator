/*XM23p emulator fetch, decode and */
/*  execution definitions          */
/*ECED 3403           JH 07 2024   */

#include "XM23p.h"
#include "execute.h"
#include "decode.h"

//IMAR, ICTRL, IMDR
wordContent instructionRegisters[IFILE];
//DMAR, DCTRL, DMBR
wordContent dataRegisters[DFILE];
//Instruction Register
code ir;
//copy of IMBR used for printing
code oldIMBR;

//Set by D0, used by E0 to determine
//which instruction to execute
//corresponds to instruction table in decodeFuncs.c
unsigned char INindex;

//copy of old instruction index
unsigned char oldIndex;

//used to flush the pipeline during branching
char bubble;

//Used to determine if a CEX state is active and if it returned 
//true/false. possible values 0,1,2 - > OFF, FALSE, TRUE
char CEXActive;

//used to track true/false counts in a CEX state
char CEXCounts[2];

//used to determine which CEX state we're currently in
//0,1,2 - > OFF, FALSE, TRUE
char CEXCurr;

//Set by F1 stage in the event of a CEX instruction
char CEXCode;


//Main execution loop calling execution stages
void execute(){
    ctrl_c_fnd = FALSE;
    while(regFile[0][PC].word != breakAddr && !ctrl_c_fnd){
        if(debugFlag) printf(" %03i", clock);
        if (!((clock+2) % 2))
        {
            if(debugFlag){
                printf("   %04x",  regFile[0][PC].word);
                printf("        %04x", memBlock[instruction].words[regFile[0][PC].word/2]);
            }
            e1();
            f0();
            d0();
            if(debugFlag){
                printf("       F0: %04x", instructionRegisters[MAR].word);
                printf("     D0: %04x", ir.value);
                if(oldIndex == LDST || oldIndex == LDRSTR) printf("    E1: %04x", oldIMBR.value);
                printf("\n");
            }
        }
        else{

            e0();
            f1();

            if(debugFlag){
                printf("            ");
                printf("              ");
                printf("F1: %04x", ir.value);
                printf("            ");
                printf("     E0: %04x\n", oldIMBR.value );
            }
        }
        clock++;
    }
}

//Set IMAR to the instruction location
//ICTRL to read
//And increment PC
void f0(){
    instructionRegisters[MAR].word = regFile[0][PC].word;
    instructionRegisters[CTRL].word = READ;
    regFile[0][PC].word+=2;
}

//Hold old IMBR, fetch new one and make a
//copy to be manipulated
//We also tackle bubbling here
void f1(){
    oldIMBR.value = instructionRegisters[MBR].word;
    instructionRegisters[MBR].word = 
            memBlock[instruction].words[instructionRegisters[MAR].word/2];

    //if there is no bubble, proceed as normal
    if(!bubble){
        ir.value = instructionRegisters[MBR].word;
        if (ir.set4.code == 2 && ir.set4.upperBit == 0)
            CEXCode = (ir.set4.PRPO<<3) + (ir.set4.DEC<<2) +
                      (ir.set4.INC<<1) + (ir.set4.WB);
    }
    else{
        bubble = 0;
    } 

    // // Decrement CEX count if required
    // if(CEXActive){
    //     //decrement the current count
    //     CEXCounts[CEXCurr-1]--;
    //     //If the current count is zero fall to the next state
    //     if(!CEXCounts[CEXCurr-1]) CEXCurr--;

    //     //Reset bubble if required
    //     if(CEXCurr != CEXActive && CEXCurr != 0) bubble = 1;

    //     //If we've run through all the instructions set CEX to inactive
    //     if(!CEXCurr) CEXActive = 0;

    // }

}

//decode stage copies contents of IR to
//a temp register then translates
//its opcode to an index
//Sets flag to identify move instructions
void d0(){
    
    oldIndex = INindex;

    if(ir.set5.up3 == 0)
        INindex = BL;
    
    else if(ir.set5.up3 == 1)
        INindex = BL+1;
    
    else if(ir.set1.opcode < GRP1)
        INindex = ir.set01.index;

    else if(ir.set1.opcode < GRP2)
        INindex = ir.set01.index+OFFSET1;

    else if(ir.set1.opcode == GRP2)
        INindex = ir.set01.rc+MOVGRP;
    
    else if(ir.set1.opcode == GRP3 && ir.set1.rc == 0)
        INindex = ir.set01.rc+MOVGRP;
    
    else if(ir.set1.opcode == GRP3 && ir.set1.rc == 1)
        INindex = ir.set01.wb+OFFSET2;

    else if(ir.set4.code == 2 && ir.set4.upperBit == 0)
        INindex = CEX;
    
    else if(ir.set4.code == 3 && ir.set4.upperBit == 0)
        INindex = LDST;
    
    else if(ir.set4.upperBit == 1)
        INindex = LDRSTR;
    
    else if(ir.set01.upopcode >= MOVGRP)
        INindex = ir.set01.upopcode + OFFSET3;
    
    else
        printf("%04x: %04x Invalid command\n", regFile[0][PC].word, ir.set1.opcode);
    
}

//takes decoded instruction
//and passes them to methods to be executed
//for ld/str sets data registers
//cases correspond to instruction table in decodeFuncs
void e0(){

    unsigned char tempByte;
    short offset;

    switch (INindex)
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
    case LDST:
        ldStHandle(ir.set4.index);
        break;
    case LDRSTR:
        tempByte = concatLdStr(ir);
        ldrStrHandle(ir.set4.index2, tempByte);
        break;
    case BL:
        offset = concatBRC(ir);
        linkBranch(ir, offset);
        break;
    case BL+1:
        offset = concatBRC(ir);
        Branch(ir, offset);
        break;
    case CEX:
        cex(ir.set4.S, ir.set4.D);
        break;
    default:
        break;
    }
}

//Second execution step used for ld/st
//Doesn't pass off to other functions like e0
void e1()
{
    if(INindex == LDST || INindex == LDRSTR){
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
        //applying post increments
        if(!oldIMBR.set4.upperBit){
            switch(oldIMBR.set4.index){
                case 0:
                    if(!oldIMBR.set4.PRPO){
                    regFile[0][oldIMBR.set4.S].word += oldIMBR.set4.INC * (2 - oldIMBR.set4.WB);
                    regFile[0][oldIMBR.set4.S].word -= oldIMBR.set4.DEC * (2 - oldIMBR.set4.WB);
                    }
                    break;
                case 1:
                    if(!oldIMBR.set4.PRPO){
                    regFile[0][oldIMBR.set4.D].word += oldIMBR.set4.INC * (2 - oldIMBR.set4.WB);
                    regFile[0][oldIMBR.set4.D].word -= oldIMBR.set4.DEC * (2 - oldIMBR.set4.WB);
                    }
                    break;
            }
        }
    }
}

//Handles setting up data pipeline registers
//flag handles which instruction
//performs pre increments
void ldStHandle(int flag)
{
    //used as a scaling factor to increment/decrement by bytes or words
    unsigned char quanta = 2 - ir.set4.WB;

    switch(flag){
        case 0:
            if(ir.set4.PRPO){
                regFile[0][ir.set4.S].word += ir.set4.INC * quanta;
                regFile[0][ir.set4.S].word -= ir.set4.DEC * quanta;
            }
            dataRegisters[MAR].word = regFile[0][ir.set4.S].word;
            break;
        case 1:
            if(ir.set4.PRPO){
                regFile[0][ir.set4.D].word += ir.set4.INC * quanta;
                regFile[0][ir.set4.D].word -= ir.set4.DEC * quanta;
            }
            dataRegisters[MAR].word = regFile[0][ir.set4.D].word;
            break;
    }
    dataRegisters[CTRL].bytes[0] = ir.set4.index;
    dataRegisters[CTRL].bytes[1] = ir.set4.WB;
}

//handles setting up data pipeline registers
//flag handles which instruction
void ldrStrHandle(int flag, char byte)
{
    switch(flag){
        case 0:
            dataRegisters[MAR].word = (signed int)regFile[0][ir.set4.S].word + (signed int)(byte);
            break;
        case 1:
            dataRegisters[MAR].word = (signed int)regFile[0][ir.set4.D].word + (signed int)(byte);
            break;
    }
    dataRegisters[CTRL].bytes[0] = ir.set4.index2;
    dataRegisters[CTRL].bytes[1] = ir.set4.WB;
}