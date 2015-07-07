#ifndef mem_h
	#define mem_h

#include <Windows.h>
#include <vector>
#include "Define.h"
#include <iostream>

using namespace std;

void MemPatch(BYTE* Dest, BYTE* Src, int Len);
void MemPatchInt(BYTE* Dest, unsigned int* Src, int Len);
void CallPatch(BYTE* Dest, BYTE* Src, int Nops);
void JumpPatch(BYTE* Dest, BYTE* Src, int Nops);
void wheelLeftPatch();
void wheelRightPatch();
void wheelLeftChangePositionX(double value);
void wheelRightChangePositionX(double value);
void patchScreenSize();
void patchSpin();


template<typename T>
 void MemPatchTemplate(BYTE* Dest, T Src, int Len){
	DWORD Old;

	VirtualProtect(Dest, Len, PAGE_EXECUTE_READWRITE, &Old);
	memcpy(Dest, Src, Len);
	VirtualProtect(Dest, Len, Old, &Old);
}

#endif