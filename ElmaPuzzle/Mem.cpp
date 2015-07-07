#include "Mem.h"


 BYTE Buff[100];

 void MemPatch(BYTE* Dest, BYTE* Src, int Len){
	DWORD Old;

	VirtualProtect(Dest, Len, PAGE_EXECUTE_READWRITE, &Old);
	memcpy(Dest, Src, Len);
	VirtualProtect(Dest, Len, Old, &Old);
}

 void MemPatchInt(BYTE* Dest, unsigned int* Src, int Len){
	DWORD Old;

	VirtualProtect(Dest, Len, PAGE_EXECUTE_READWRITE, &Old);
	memcpy(Dest, Src, Len);
	VirtualProtect(Dest, Len, Old, &Old);
}

 

 void CallPatch(BYTE* Dest, BYTE* Src, int Nops){
	Buff[0] = 0xE8;	
	*(int*)&Buff[1] = Src-(Dest+5);

	for(int i = 5; i < Nops+5; i++){
		Buff[i] = 0x90;
	}
	MemPatch(Dest, Buff, Nops+5);
}


 void JumpPatch(BYTE* Dest, BYTE* Src, int Nops){
	Buff[0] = 0xE9;	
	*(int*)&Buff[1] = Src-(Dest+5);

	for(int i = 5; i < Nops+5; i++){
		Buff[i] = 0x90;
	}
	MemPatch(Dest, Buff, Nops+5);
}

 void wheelLeftPatch(){
	BYTE* Dest = (BYTE*)0x004019e2;
	for(int i = 0 ; i < 10 ; i++)
		Buff[i] = 0x90;
	MemPatch(Dest,Buff,10);
	Dest = (BYTE*)0x004019ec;
	MemPatch(Dest,Buff,10);
	Dest = (BYTE*)0x00401cd9;
	MemPatch(Dest,Buff,5);
	Dest = (BYTE*)0x00401cec;
	MemPatch(Dest,Buff,6);
}

 void wheelRightPatch(){
	BYTE* Dest = (BYTE*)0x00401a0a;
	for(int i = 0 ; i < 10 ; i++)
		Buff[i] = 0x90;
	MemPatch(Dest,Buff,10);
	Dest = (BYTE*)0x00401a14;
	MemPatch(Dest,Buff,10);
	Dest = (BYTE*)0x00401d4b;
	MemPatch(Dest,Buff,5);
	Dest = (BYTE*)0x00401d5e;
	MemPatch(Dest,Buff,6);
}

 void wheelLeftChangePositionX(double value){
	BYTE* Dest = (BYTE*)0x004537d0;
	double d = value;
	BYTE* byteArray = reinterpret_cast<BYTE*>(&d);
	MemPatch(Dest,byteArray,8);
}

 void wheelRightChangePositionX(double value){
	BYTE* Dest = (BYTE*)0x004537e8;
	double d = value;
	BYTE* byteArray = reinterpret_cast<BYTE*>(&d);
	MemPatch(Dest,byteArray,8);
}

void patchSpin()
{
	for(int i = 0 ; i < 10 ; i++)
		Buff[i] = 0x90;
	BYTE* Dest = (BYTE*)0x00401AAA;
	MemPatch(Dest,Buff,10);
	Dest = (BYTE*)0x00401AB4;
	MemPatch(Dest,Buff,10);
}

void patchScreenSize()
{
	for(int i = 0 ; i < 6 ; i++)
		Buff[i] = 0x90;

	BYTE* Dest = (BYTE*)0x0041B5CC;
	MemPatch(Dest,Buff,6);
	Dest = (BYTE*)0x0041B57C;
	MemPatch(Dest,Buff,6);
	
}

