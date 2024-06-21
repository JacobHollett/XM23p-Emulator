/*XM23p instruction definitions    */
/*ECED 3403              JH 2024   */

#include "XM23p.h"

unsigned char overflowTable[2][2][2] = {{{0,1},{0,0}},
                                        {{0,0},{1,0}}};


//Handles the add and addc
//carry is passed in as 0 for add
void ADD(int RC, int WB, int SC, int D, int carry){

    twoWord tempResult;

    if (WB){
        tempResult.value = regFile[RC][SC].bytes[0] + regFile[0][D].bytes[0] + carry;
        
        psw.bit[0].b0 = tempResult.bit[1].b0;   //Carry
        if (!tempResult.bytes[0])               //Zero
            psw.bit[0].b1 = 1;
        else
            psw.bit[0].b1 = 0;
        psw.bit[0].b2 = tempResult.bit[0].b7;   //Negative
        psw.bit[0].b4 = 
        overflowTable[regFile[RC][SC].bit[0].b7][regFile[0][D].bit[0].b7][tempResult.bit[0].b7];
        regFile[0][D].bytes[0] = tempResult.bytes[0];
        
    }
    else{
        tempResult.value = regFile[RC][SC].word + regFile[0][D].word + carry;
        
        psw.bit[0].b0 = tempResult.bit[2].b0;   //Carry
        if (!tempResult.word[0])                //Zero
            psw.bit[0].b1 = 1;
        else
            psw.bit[0].b1 = 0; 
        psw.bit[0].b2 = tempResult.bit[1].b7;   //Negative
        psw.bit[0].b4 = 
        overflowTable[regFile[RC][SC].bit[1].b7][regFile[0][D].bit[1].b7][tempResult.bit[1].b7];
        regFile[0][D].word = tempResult.word[0];
    }
}

//handles sub and subc
void SUB(int RC, int WB, int SC, int D, int carry, int flag){

    twoWord tempResult;

    if (WB){
        tempResult.value = regFile[0][D].bytes[0] - regFile[RC][SC].bytes[0];
        if(flag)
            tempResult.value = tempResult.value - 1 + carry;
        
        psw.bit[0].b0 = tempResult.bit[1].b0;   //Carry
        if (!tempResult.bytes[0])               //Zero
            psw.bit[0].b1 = 1;
        else
            psw.bit[0].b1 = 0;
        psw.bit[0].b2 = tempResult.bit[0].b7;   //Negative
        psw.bit[0].b4 = 
        overflowTable[regFile[RC][SC].bit[0].b7][regFile[0][D].bit[0].b7][tempResult.bit[0].b7];
        regFile[0][D].bytes[0] = tempResult.bytes[0];
        
    }
    else{
        tempResult.value = regFile[0][D].word - regFile[RC][SC].word;
        if(flag)
            tempResult.value = tempResult.value - 1 + carry;
        
        psw.bit[0].b0 = tempResult.bit[2].b0;   //Carry
        if (!tempResult.word[0])                //Zero
            psw.bit[0].b1 = 1;
        else
            psw.bit[0].b1 = 0; 
        psw.bit[0].b2 = tempResult.bit[1].b7;   //Negative
        psw.bit[0].b4 = 
        overflowTable[regFile[RC][SC].bit[1].b7][regFile[0][D].bit[1].b7][tempResult.bit[1].b7];
        regFile[0][D].word = tempResult.word[0];
    }
}