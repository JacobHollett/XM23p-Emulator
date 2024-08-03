/*XM23p instruction definitions    */
/*Implements each of the XM23p     */
/*instructions                     */
/*ECED 3403           JH 07 2024   */

#include "XM23p.h"
#include "execute.h"

unsigned char overflowTable[2][2][2] = {{{0,1},{0,0}},
                                        {{0,0},{1,0}}};

//Called by each arithmetic instructions that accesses all
//PSW bits, performs checks on result and operands
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


void linkBranch(code strction, short offset){
    //subtract two since f0 will have bumped up the PC
    regFile[0][LR].word = regFile[0][PC].word - 2;
    regFile[0][PC].word += (offset-2);
    //insert NOP into the ir and skip the next decode phase
    ir.value = NOP;
    bubble = 1;
}

//handles each branch instruction aside from branch with link
void Branch(code strction, short offset){

    //indicated branch success
    char success = 0;

    switch(strction.set5.low3){
        case 0:
            if(psw.bit[0].b1) success = 1;
            break;
        case 1:
            if(!psw.bit[0].b1) success = 1;
            break;
        case 2:
            if(psw.bit[0].b0) success = 1;
            break;
        case 3:
            if(!psw.bit[0].b0) success = 1;
            break;
        case 4:
            if(psw.bit[0].b2) success = 1;
            break;
        case 5:
            if(psw.bit[0].b2 ^ (!psw.bit[0].b4))
                success = 1;
            break;
        case 6:
            if(psw.bit[0].b2 ^ psw.bit[0].b4)
                success = 1;
            break;
        case 7:
            success = 1;
            break;
    }

    if (success)
    {
        //again accounting for the fact we've already bumped
        //the PC forward
        regFile[0][PC].word += (offset-2);
        bubble = 1;
        ir.value = NOP;
    }
}

//Sets CEX state based on condition code
//and t/f counts
void cex(char true, char false){
    //used to determine which part is executed (t/f)
    int i = 0;

    switch(CEXCode){
        
        case EQ:
            if(psw.bit[0].b1)
                i = 1;
            break;
        case NE:
            if(!psw.bit[0].b1)
                i = 1;
            break;
        case CS:
            if(psw.bit[0].b0)
                i = 1;
            break;
        case CC:
            if(!psw.bit[0].b0)
                i = 1;
            break;
        case MI:
            if(psw.bit[0].b2)
                i = 1;
            break;
        case PL:
            if(!psw.bit[0].b2)
                i = 1;
            break;
        case VS:
            if(psw.bit[0].b4)
                i = 1;
            break;
        case VC:
            if(!psw.bit[0].b4)
                i = 1;
            break;
        case HI:
            if(psw.bit[0].b0 && !psw.bit[0].b1)
                i = 1;
            break;
        case LS:
            if(!psw.bit[0].b0 || psw.bit[0].b1)
                i = 1;
            break;
        case GE:
            if(psw.bit[0].b2 == psw.bit[0].b4)
                i = 1;
            break;
        case LT:
            if(psw.bit[0].b2 != psw.bit[0].b4)
                i = 1;
            break;
        case GT:
            if(!psw.bit[0].b1 && (psw.bit[0].b4 == psw.bit[0].b2))
                i = 1;
            break;
        case LE:
            if(psw.bit[0].b1 || (psw.bit[0].b4 != psw.bit[0].b2))
                i = 1;
            break;
        case TR:
            i = 1;
            break;
        case FL:
            i = 0;
            break;
    }

    CEXCounts[0] = true;
    CEXCounts[1] = false;
    CEXActive = i+1;

}