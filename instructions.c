/*XM23p instruction definitions    */
/*ECED 3403              JH 2024   */

#include "XM23p.h"

unsigned char overflowTable[2][2][2] = {{{0,1},{0,0}},
                                        {{0,0},{1,0}}};


void setPSW(twoWord tempResult, int WB, int RC, int SC, int D){

    if (WB){
        psw.bit[0].b0 = tempResult.bit[1].b0;   //Carry
        if (!tempResult.bytes[0])               //Zero
            psw.bit[0].b1 = 1;
        else
            psw.bit[0].b1 = 0;
        psw.bit[0].b2 = tempResult.bit[0].b7;   //Negative
        psw.bit[0].b4 = 
        overflowTable[regFile[RC][SC].bit[0].b7][regFile[0][D].bit[0].b7][tempResult.bit[0].b7];
    }
    else{
        psw.bit[0].b0 = tempResult.bit[2].b0;   //Carry
        if (!tempResult.word[0])                //Zero
            psw.bit[0].b1 = 1;
        else
            psw.bit[0].b1 = 0;
        psw.bit[0].b2 = tempResult.bit[1].b7;   //Negative
        psw.bit[0].b4 = 
        overflowTable[regFile[RC][SC].bit[1].b7][regFile[0][D].bit[1].b7][tempResult.bit[1].b7];
    }
}


//Handles the add and addc
//carry is passed in as 0 for add
void ADD(int RC, int WB, int SC, int D, int carry){

    twoWord tempResult;

    if (WB){
        tempResult.value = regFile[RC][SC].bytes[0] + regFile[0][D].bytes[0] + carry;
        setPSW(tempResult, WB, RC, SC, D);
        regFile[0][D].bytes[0] = tempResult.bytes[0];
    }
    else{
        tempResult.value = regFile[RC][SC].word + regFile[0][D].word + carry;
        setPSW(tempResult, WB, RC, SC, D);
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
        setPSW(tempResult, WB, RC, SC, D);
        regFile[0][D].bytes[0] = tempResult.bytes[0];
    }
    else{
        tempResult.value = regFile[0][D].word - regFile[RC][SC].word;
        if(flag)
            tempResult.value = tempResult.value - 1 + carry;
        setPSW(tempResult, WB, RC, SC, D);
        regFile[0][D].word = tempResult.word[0];
    }
}


void DADD(int RC, int WB, int SC, int D){

    wordContent tempResult;
    tempResult.word = 0;

    for (int i = 0; i <= 1-WB; i++) {
        tempResult.digits[i].n0 = (regFile[RC][SC].digits[i].n0 + regFile[0][D].digits[i].n0 + psw.bit[0].b0);
        if (tempResult.digits[i].n0 >= 10){
            tempResult.digits[i].n0-=10;
            psw.bit[0].b0 = 1;  //carry
        }
        else psw.bit[0].b0 = 0; //carry

        tempResult.digits[i].n1 = (regFile[RC][SC].digits[i].n1 + regFile[0][D].digits[i].n1 + psw.bit[0].b0);
        if (tempResult.digits[i].n1 >= 10){
            tempResult.digits[i].n1-=10;
            psw.bit[0].b0 = 1;  //carry
        }
        else psw.bit[0].b0 = 0; //carry
    }
    //zero
    if (WB){
        regFile[0][D].bytes[0] = tempResult.bytes[0];
        if(!tempResult.bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
    } 
    else{
        regFile[0][D].word = tempResult.word;
        if(!tempResult.word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
    }
    psw.bit[0].b2 = 0; //negative
    psw.bit[0].b4 = 0; //overflow
    
}

//Identical process to subtract,
//result is simply not saved
void CMP(int RC, int WB, int SC, int D){
    
    twoWord tempResult;

    if (WB) tempResult.value = regFile[0][D].bytes[0] - regFile[RC][SC].bytes[0];
    else tempResult.value = regFile[0][D].word - regFile[RC][SC].word;
    
    setPSW(tempResult, WB, RC, SC, D);
}


void XOR(int RC, int WB, int SC, int D){

    if (WB){
        regFile[0][D].bytes[0] = regFile[0][D].bytes[0] ^ regFile[RC][SC].bytes[0];
        if(!regFile[0][D].bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[0].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
    else{
        regFile[0][D].word = regFile[0][D].word ^ regFile[RC][SC].word;
        if(!regFile[0][D].word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[1].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
}


void AND(int RC, int WB, int SC, int D){

    if (WB){
        regFile[0][D].bytes[0] = regFile[0][D].bytes[0] & regFile[RC][SC].bytes[0];
        if(!regFile[0][D].bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[0].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    } 
    else{
        regFile[0][D].word = regFile[0][D].word & regFile[RC][SC].word;
        if(!regFile[0][D].word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[1].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
}


void OR(int RC, int WB, int SC, int D){

    if (WB){
        regFile[0][D].bytes[0] = regFile[0][D].bytes[0] | regFile[RC][SC].bytes[0];
        if(!regFile[0][D].bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[0].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
    else{
        regFile[0][D].word = regFile[0][D].word | regFile[RC][SC].word;
        if(!regFile[0][D].word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[1].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
}

void BIT(int RC, int WB, int SC, int D){

    wordContent tempWord;

    if (WB){
        tempWord.bytes[0] = regFile[0][D].bytes[0] & (1<<regFile[RC][SC].bytes[0]);
        if (!tempWord.bytes[0]) psw.bit[0].b1 = 1;  //zero
        else psw.bit[0].b1 = 0;
    }
    else{
        tempWord.word = regFile[0][D].word & (1<<regFile[RC][SC].word);
        if (!tempWord.word) psw.bit[0].b1 = 1;  //zero
        else psw.bit[0].b1 = 0;
    }

}

void BIC(int RC, int WB, int SC, int D){

    unsigned short temp;

    if (WB){
        temp = 1<<regFile[RC][SC].bytes[0];
        regFile[0][D].bytes[0] = regFile[0][D].bytes[0] & ~(temp);
        if (!regFile[0][D].bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[0].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
    else{
        temp = 1<<regFile[RC][SC].word;
        regFile[0][D].word = regFile[0][D].word & ~(temp);
        if (!regFile[0][D].word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[1].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
}


void BIS(int RC, int WB, int SC, int D){

    if (WB){
        regFile[0][D].bytes[0] = regFile[0][D].bytes[0] | (1<<regFile[RC][SC].bytes[0]);
        if (!regFile[0][D].bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[0].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
    else{
        regFile[0][D].word = regFile[0][D].word | (1<<regFile[RC][SC].word);
        if (!regFile[0][D].word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;
        if(regFile[0][D].bit[1].b7) psw.bit[0].b2 = 1;
        else psw.bit[0].b2 = 0;
    }
}


void MOV(int WB, int SC, int D){

    if (WB) regFile[0][D].bytes[0] = regFile[0][SC].bytes[0];
    else regFile[0][D].word = regFile[0][SC].word;
}

void SWAP(int SC, int D){

    wordContent tempWord;
    tempWord.word = regFile[0][SC].word;
    regFile[0][SC].word = regFile[0][D].word;
    regFile[0][D].word = tempWord.word;
}


void SRA(int WB, int D){

    psw.bit[0].b0 = regFile[0][D].bit[0].b0;        //carry
    if(WB){
        regFile[0][D].bytes[0] = regFile[0][D].bytes[0]>>1;
        if(!regFile[0][D].bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;                     //zero
        psw.bit[0].b2 = regFile[0][D].bit[0].b7;    //neg
    }
    else{
        regFile[0][D].word = regFile[0][D].word>>1;
        if(!regFile[0][D].word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;                     //zero
        psw.bit[0].b2 = regFile[0][D].bit[1].b7;    //neg
    }
}


void RRC(int WB, int D){

    wordContent tempCarry;
    tempCarry.bit[0].b0 = psw.bit[0].b0;       //holding old carry
    psw.bit[0].b0 = regFile[0][D].bit[0].b0;        //carry
    if(WB){
        regFile[0][D].bytes[0] = regFile[0][D].bytes[0]>>1;
        regFile[0][D].bit[0].b7 = tempCarry.bit[0].b0;
        if(!regFile[0][D].bytes[0]) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;                     //zero
        psw.bit[0].b2 = regFile[0][D].bit[0].b7;    //neg
    }
    else{
        regFile[0][D].word = regFile[0][D].word>>1;
        regFile[0][D].bit[1].b7 = tempCarry.bit[0].b0;
        if(!regFile[0][D].word) psw.bit[0].b1 = 1;
        else psw.bit[0].b1 = 0;                     //zero
        psw.bit[0].b2 = regFile[0][D].bit[1].b7;    //neg
    }
}

void SWPB(int D){

    unsigned char tempByte;
    tempByte = regFile[0][D].bytes[0];
    regFile[0][D].bytes[0] = regFile[0][D].bytes[1];
    regFile[0][D].bytes[1] = tempByte;
    if (!regFile[0][D].word)                //Zero
        psw.bit[0].b1 = 1;
    else
        psw.bit[0].b1 = 0;
    psw.bit[0].b2 = regFile[0][D].bit[1].b7;   //Negative
}


void SXT(int D){

    if (regFile[0][D].bit[0].b7)
        regFile[0][D].bytes[1] = 0xFF;
    else
        regFile[0][D].bytes[1] = 0;
    if (!regFile[0][D].word)                //Zero
        psw.bit[0].b1 = 1;
    else
        psw.bit[0].b1 = 0;
    psw.bit[0].b2 = regFile[0][D].bit[1].b7;   //Negative

}


void MOVL(int D, unsigned char byte){

    regFile[0][D].bytes[0] = byte;

}


void MOVLZ(int D, unsigned char byte){

    regFile[0][D].bytes[0] = byte;
    regFile[0][D].bytes[1] = 0x00;

}


void MOVLS(int D, unsigned char byte){

    regFile[0][D].bytes[0] = byte;
    regFile[0][D].bytes[1] = 0xFF;
}


void MOVH(int D, unsigned char byte)
{
    regFile[0][D].bytes[1] = byte;
}

void SETCC(int V, int SLP, int N, int Z, int C)
{
    if(C) psw.bit[0].b0 = 1;
    if(Z) psw.bit[0].b1 = 1;
    if(N) psw.bit[0].b2 = 1;
    if(SLP) psw.bit[0].b3 = 1;
    if(V) psw.bit[0].b4 = 1;
}

void CLRCC(int V, int SLP, int N, int Z, int C)
{
    if(C) psw.bit[0].b0 = 0;
    if(Z) psw.bit[0].b1 = 0;
    if(N) psw.bit[0].b2 = 0;
    if(SLP) psw.bit[0].b3 = 0;
    if(V) psw.bit[0].b4 = 0;
}