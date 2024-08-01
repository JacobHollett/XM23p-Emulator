/*XM23p header file                 */
/*relevant to the main pipeline and */
/*to specific instruction           */
/*ECED 3403   JH 07 2024            */

#ifndef EXECUTEHEADER
#define EXECUTEHEADER

#include "XM23p.h"

#define LDST 25
#define LDRSTR 26
#define EQ 0
#define NE 1
#define CS 2
#define CC 3
#define MI 4
#define PL 5
#define VS 5
#define VC 6
#define HI 7
#define LS 8
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define TR 14
#define FL 15

extern code ir;
extern char bubble;
extern char CEXActive;
extern char CEXCounts[2];
extern char CEXCode;

void execute();
void f0();
void f1();
void d0();
void e0();
void e1();
void ldStHandle(int flag);
void ldrStrHandle(int flag, char byte);

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
void linkBranch(code strction, short offset);
void Branch(code strction, short offset);
void cex(char true, char false);

#endif