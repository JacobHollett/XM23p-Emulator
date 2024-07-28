/*XM23p decode relevant header file */
/*Contains function prototypes      */
/*ECED 3403   JH 07 2024            */

#ifndef DECODEHEADER
#define DECODEHEADER

#include "XM23p.h"

void decodeInstructions();
void printInstruction(int index, int wb, int rc, int src, int d, int flag);
void printMoves(int index, unsigned char byte, int d);
void printConCodes(int index, code strction);
void printBranches(int index, short offset);
void printLdStr(int flag, int index, code strction);
unsigned char concatByte(unsigned char b1, unsigned char b2);
char concatLdStr(code strction);
short concatBRC(code strction);

#endif