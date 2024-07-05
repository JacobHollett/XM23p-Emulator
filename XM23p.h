/*XM23p Loader header file      */
/*Contains definitions,includes */
/*and function prototypes       */
/*ECED 3403   JH 2024           */
#pragma once

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MEMSIZE (1<<16)
#define data 1              //DMEM index
#define instruction 0       //IMEM index
#define MAXRECORDLENG 75    //2+2+4+60+2+EOF+6 extra
#define CHECKSUM 0xFF
#define REGFILE 8
#define REGCON 2
#define IFILE 3
#define DFILE 3
#define numStructions 23
#define MAR 0
#define MBR 1
#define CTRL 2
#define READ 0
#define write 1


typedef union memories
{
    unsigned char bytes[MEMSIZE];
    unsigned short words[MEMSIZE/2];
} memory;

typedef struct nibbles
{
    unsigned char n0:4;
    unsigned char n1:4;
} nibbles;

typedef struct bits
{
    unsigned char b0:1; //LSB
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
    unsigned char b6:1;
    unsigned char b7:1; //MSB
} bits;

typedef struct group1
{
    unsigned char d:3;
    unsigned char sc:3;
    unsigned char wb:1;
    unsigned char rc:1;
    unsigned char opcode;
} group1;

typedef struct subGroup1
{
    unsigned char d:3;
    unsigned char sc:3;
    unsigned char wb:1;
    unsigned char rc:1;
    unsigned char index:3;
    unsigned char upopcode:5;
} subGroup1;

typedef struct group2
{
    unsigned char d:3;
    unsigned char b1:5;
    unsigned char b2:3;
    unsigned char opcode:5;
} group2;

typedef struct setClrGrp
{   
    unsigned char C:1;
    unsigned char Z:1;
    unsigned char N:1;
    unsigned char SLP:1;
    unsigned char V:1;
    unsigned char b56:2;
    unsigned char b7:1;
    unsigned char opcode;
} setClrGrp;

typedef struct LdStGrp
{
    unsigned char D:3;
    unsigned char S:3;
    unsigned char WB:1;
    unsigned char INC:1;
    unsigned char DEC:1;
    unsigned char PRPO:1;
    unsigned char index:1;
    unsigned char code:3;
    unsigned char index2:1;
    unsigned char upperBit:1;
} LdStGrp;

typedef union code
{
    unsigned short value;
    group1 set1;
    subGroup1 set01;
    group2 set2;
    setClrGrp set3;
    LdStGrp set4;
} code;

typedef union nibbles_bytes_words
{
    unsigned short word;
    unsigned char bytes[2];
    nibbles digits[2];
    bits bit[2];
} wordContent;

typedef union twoWord
{
    unsigned int value;
    unsigned short word[2];
    unsigned char bytes[4];
    bits bit[4];
} twoWord;

extern char sourceFileName[];
extern int startingAddress;
extern int clock;
extern unsigned char debugFlag;
extern memory memBlock[];
extern wordContent regFile[REGCON][REGFILE];
extern wordContent instructionRegisters[IFILE];
extern wordContent dataRegisters[DFILE];
extern wordContent psw;
extern unsigned int breakAddr;
extern code ir;

FILE *openFile();
void readRecords(FILE *inputFile);
void parseS0(char **recordPtr);
void parseS12(char **recordPtr, int flag);
void parseS9(char **recordPtr);
unsigned char hexToByte(unsigned char char1, unsigned char char2);
void displayMem();
void initializePipelines();

void decodeInstructions();
void printInstruction(int index, int wb, int rc, int src, int d, int flag);
void printMoves(int index, unsigned char byte, int d);
void printConCodes(int index, code strction);
void printLdStr(int flag, int index, code strction);
unsigned char concatByte(unsigned char b1, unsigned char b2);
unsigned char concatLdStr(code strction);

void printHeader();
void displayRegisters();
void changeRegister();
void changeMEM();
void setBreak();
void displayPSW();

void execute();
void f0();
void f1();
void d0();
void e0();
void e1();

void ADD(int RC, int WB, int SC, int D, int carry);
void SUB(int RC, int WB, int SC, int D, int carry, int flag);
void DADD(int RC, int WB, int SC, int D);
void CMP(int RC, int WB, int SC, int D);
void XOR(int RC, int WB, int SC, int D);
void AND(int RC, int WB, int SC, int D);
void OR(int RC, int WB, int SC, int D);
void BIT(int RC, int WB, int SC, int D);
void BIC(int RC, int WB, int SC, int D);
void BIS(int RC, int WB, int SC, int D);
void MOV(int WB, int SC, int D);
void SWAP(int SC, int D);
void SRA(int WB, int D);
void RRC(int WB, int D);
void SWPB(int D);
void SXT(int D);
void MOVL(int D, unsigned char byte);
void MOVLZ(int D, unsigned char byte);
void MOVLS(int D, unsigned char byte);
void MOVH(int D, unsigned char byte);
void SETCC(int V, int SLP, int N, int Z, int C);
void CLRCC(int V, int SLP, int N, int Z, int C);