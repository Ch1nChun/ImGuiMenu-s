#ifndef _DETOURS_H
#define _DETOURS_H

extern "C" int  __cdecl		mlde32(void *codeptr);

int   DetourLen(BYTE *src, int minlen);
void *DetourCreate(BYTE *src, const BYTE *dst, const int minlen=0);
void  DetourRemove(BYTE *src, BYTE *restore, const int len=0);

#endif