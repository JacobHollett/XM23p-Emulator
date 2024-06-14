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


typedef union memories{

    unsigned char bytes[MEMSIZE];
    unsigned short words[MEMSIZE/2];

} memory;

typedef struct nibbles{

    unsigned short n0:4;
    unsigned short n1:4;
    unsigned short n2:4;
    unsigned short n3:4;

} nibbles;

typedef union nibbles_bytes_words{

    unsigned short word;
    unsigned char bytes[2];
    struct nibbles digits;

} wordContent;


extern char sourceFileName[];
extern int startingAddress;
extern memory memBlock[];
extern wordContent regFile[REGCON][REGFILE];
extern unsigned int breakAddr;

extern char instructionSet[][4][6];


void readRecords(FILE *inputFile);
void parseS0(char **recordPtr);
void parseS12(char **recordPtr, int flag);
void parseS9(char **recordPtr);
unsigned char hexToByte(unsigned char char1, unsigned char char2);
void displayMem();

void decodeInstructions();
void printInstruction(int address, int set, int index, int struction);

void displayRegisters();
void changeRegister();
void changeMEM();
void setBreak();