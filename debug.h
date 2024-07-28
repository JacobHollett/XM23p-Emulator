/*XM23p debug relevant header file */
/*Contains function prototypes     */
/*ECED 3403   JH 07 2024           */

#ifndef DEBUGHEADER
#define DEBUGHEADER

void printHeader();
void displayRegisters();
void changeRegister();
void changeMEM();
void setBreak();
void displayPSW();
void sigint_hdlr();

#endif