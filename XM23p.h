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
#define numStructions 23
#define IMAR 0
#define IMBR 1
#define ICTRL 2
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
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
    unsigned char b6:1;
    unsigned char b7:1;
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

typedef union code
{
    unsigned short value;
    group1 set1;
    subGroup1 set01;
    group2 set2;
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
extern memory memBlock[];
extern wordContent regFile[REGCON][REGFILE];
extern wordContent instructionRegisters[IFILE];
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

void decodeInstructions();
void printInstruction(int index, int wb, int rc, int src, int d, int flag);
void printMoves(int index, int byte, int d);
unsigned int concatByte(unsigned char b1, unsigned char b2);

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

void ADD(int RC, int WB, int SC, int D, int carry);
void SUB(int RC, int WB, int SC, int D, int carry, int flag);
void DADD(int RC, int WB, int SC, int D);
void CMP(int RC, int WB, int SC, int D);