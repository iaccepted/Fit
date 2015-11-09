#ifndef _SHEVAL_H_
#define _SHEVAL_H_

#include "Global.h"

//Evaluate an Associated Legendre Polynomial P(l, m) at x
LFLOAT P(int l, int m, LFLOAT x);

//Calculate the normalisation constant for an SH function
LFLOAT K(int l, int m);

//Sample a spherical harmonic basis function Y(l, m) at a point on the unit sphere
LFLOAT SH(int l, int m, LFLOAT theta, LFLOAT phi);

//Calculate n!
int Factorial(int n);


typedef void(*SHEvalFunc)(const LFLOAT, const LFLOAT, const LFLOAT, LFLOAT*);
void SHEval3(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval4(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval5(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval6(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval7(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval8(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval9(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);
void SHEval10(const LFLOAT fX, const LFLOAT fY, const LFLOAT fZ, LFLOAT *pSH);

#endif