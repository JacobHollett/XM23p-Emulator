/*XM23p loader relevant header file*/
/*Contains function declarations   */
/*ECED 3403   JH 07 2024           */

#ifndef LOADHEADER
#define LOADHEADER

#include "XM23p.h"

FILE *openFile();
void readRecords(FILE *inputFile);
void parseS0(char **recordPtr);
void parseS12(char **recordPtr, int flag);
void parseS9(char **recordPtr);
unsigned char hexToByte(unsigned char char1, unsigned char char2);
void displayMem();
void initializePipelines();

#endif