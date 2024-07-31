/*XM23p header file           */
/*Contains general definitions*/
/*& code structures           */
/*ECED 3403   JH 07 2024      */
#pragma once
#ifndef MAINHEADER
#define MAINHEADER

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#define MEMSIZE (1<<16)
#define data 1              //DMEM index
#define instruction 0       //IMEM index
#define MAXRECORDLENG 75    //2+2+4+60+2+EOF+6 extra
#define CHECKSUM 0xFF
#define REGFILE 8
#define REGCON 2
#define IFILE 3
#define DFILE 3
#define maxInstruction 10
#define MAR 0
#define MBR 1
#define CTRL 2
#define READ 0
#define write 1
#define PC 7
#define LR 5
#define NOP 0x4C00
#define TRUE 1
#define FALSE 0

#define GRP1 0x48
#define GRP2 0x4C
#define GRP3 0x4d
#define MOVGRP 0xc
#define OFFSET1 0x8
#define OFFSET2 0x17
#define OFFSET3 0x7
#define OFFSET4 0x19
#define BL 29
#define CEX 38

/*
 * XM23p Memory byte/word representation
*/
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

typedef struct branchGrp
{
    unsigned char off1;
    unsigned char off2:2;
    unsigned char low3:3;
    unsigned char up3:3;
} branchGrp;

/*
 * Representative of instructions
*/
typedef union code
{
    unsigned short value;
    group1 set1;
    subGroup1 set01;
    group2 set2;
    setClrGrp set3;
    LdStGrp set4;
    branchGrp set5;
} code;

typedef union nibbles_bytes_words
{
    unsigned short word;
    unsigned char bytes[2];
    nibbles digits[2];
    bits bit[2];
} wordContent;

/*
 * Used to access registers as needed
*/
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
extern volatile sig_atomic_t ctrl_c_fnd; /* T|F - indicates whether ^C detected */

#endif