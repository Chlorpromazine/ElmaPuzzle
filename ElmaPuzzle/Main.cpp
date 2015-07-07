#include <stdio.h>
#include <string.h>
#include <iostream>
#include <Windows.h>
#include <fstream>

#include "Mem.h"
#include "objectHack.h"
#include "misc.h"

//debug
#include "guicon.h"

#include <crtdbg.h>
#include <conio.h>

using namespace std;

void hotKeys();
void displayStatus();
int run = 2;

static vector<bool> currKeyStates;
static vector<bool> prevKeyStates;

DWORD WINAPI mainLoop(void*){

	misc::mC.disableMovement();
	misc::mC.invulnerable();

	for (int i = 0; i < 255; i++)
	{
		currKeyStates.push_back(0);
		prevKeyStates.push_back(0);
	}

	while (run == 2)
	{
		//0 = menu, 1 = in normal level, replay = 2, 3 = editor level
		switch (currentScreen)
		{
		case 0:
			objects::obj.reload = 1;
			break;
		case 1:
		case 3:
			//every time you leave a level
			if (objects::obj.reload) objects::obj.clearVals();

			hotKeys();
			Sleep(10);
			
			break;
		case 2:
			break;
		}
		Sleep(1);
	}

	return 0;
}



void keyDown(int key1)
{
	if (GetAsyncKeyState(key1) < 0)
		currKeyStates[key1] = 1;
	else currKeyStates[key1] = 0;
}

void hotKeys()
{

	//next object (w)
	keyDown(0x57);
	if (prevKeyStates[0x57] != currKeyStates[0x57])
	{
		if (currKeyStates[0x57])
		{
			if (misc::mC.canMove())
			{
				misc::mC.setMove(0);
				misc::mC.movementX = 0;
				misc::mC.movementY = 0.1;
			}
		}

	}prevKeyStates[0x57] = currKeyStates[0x57];

	//next object (a)
	keyDown(0x41);
	if (prevKeyStates[0x41] != currKeyStates[0x41])
	{
		if (currKeyStates[0x41])
		{
			if (misc::mC.canMove())
			{
				misc::mC.setMove(0);
				misc::mC.movementX = -0.1;
				misc::mC.movementY = 0;
			}
		}
			

	}prevKeyStates[0x41] = currKeyStates[0x41];

	//next object (s)
	keyDown(0x53);
	if (prevKeyStates[0x53] != currKeyStates[0x53])
	{
		if (currKeyStates[0x53])
		{
			if (misc::mC.canMove())
			{
				misc::mC.setMove(0);
				misc::mC.movementX = 0;
				misc::mC.movementY = -0.1;
			}
		}
			

	}prevKeyStates[0x53] = currKeyStates[0x53];

	//next object (d)
	keyDown(0x44);
	if (prevKeyStates[0x44] != currKeyStates[0x44])
	{
		if (currKeyStates[0x44])
		{
			if (misc::mC.canMove())
			{
				misc::mC.setMove(0);
				misc::mC.movementX = 0.1;
				misc::mC.movementY = 0;
			}
		}
			

	}prevKeyStates[0x44] = currKeyStates[0x44];

	//remove dll from elma (DEL)
	if (GetAsyncKeyState(VK_DELETE))
	{
		run = 1;
	}

}

int addr_0049D140 = 0x0049D140;
int addr_0049D160 = 0x0049D160;
void __declspec(naked) antiCrash()
{
	//the registries are different when playing an internal level compared to external
	if (isInternalLev) //if 1-55, internal, 0 else
		__asm{

			mov eax, 0x0000000D
			mov ebx, 0x00000038
			mov ecx, 0x0018f9d8
			mov edx, 0x03034498
			mov esi, 0x00000000
			mov edi, 0x00000001
			mov esp, 0x0018f9bc

			jmp addr_0049D140
	}
	else
		__asm{

			mov eax, 0x0000000D
			mov ebx, 0x00001737
			mov ecx, 0x0018f9d8
			mov edx, 0x02f64498
			mov esi, 0x0018f931
			mov edi, 0x07131945
			mov ebp, 0x00001737
			mov esp, 0x0018f908

			jmp addr_0049D160
	}

	//todo: fix bug for crashing when out of bound from the bottom side.
	//todo: remove crash for more than one start

}

BOOL WINAPI DllMain(HMODULE hDllHandle, DWORD nReason, LPVOID lpReserved)
{
	if (nReason == DLL_PROCESS_ATTACH)
	{
		//debug console
		RedirectIOToConsole();
		run = 2;
		
		//grab object data
		JumpPatch((BYTE*)0x00408DEF, (BYTE*)&objInject, 1);
		//grab collision data
		JumpPatch((BYTE*)0x0042E70E, (BYTE*)&collisionObjInject, 0);
		//go in main physics loop
		
		JumpPatch((BYTE*)0x00495a89, (BYTE*)&moveBikeInject, 0);

		//removes crashes for wheels being too far from bike or bike going out of bound.
		JumpPatch((BYTE*)0x00430760, (BYTE*)&antiCrash, 1);
		JumpPatch((BYTE*)0x0043066F, (BYTE*)&antiCrash, 1);
		JumpPatch((BYTE*)0x0040A96A, (BYTE*)&antiCrash, 1);
		JumpPatch((BYTE*)0x0042C2BB, (BYTE*)&antiCrash, 1);
		JumpPatch((BYTE*)0x00430740, (BYTE*)&antiCrash, 1);

		//remove + and - functions from elma, they're useless
		patchScreenSize();

		//create thread for main part of hack
		CreateThread(NULL, 0, mainLoop, NULL, 0L, NULL);
	}
	else if (nReason == DLL_PROCESS_DETACH)
	{
		run = 1;
	}

	return 1;
}



