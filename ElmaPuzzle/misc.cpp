#include "misc.h"
#include <Windows.h>
#include "Mem.h"

misc misc::mC;

misc::misc()
{
	//turned on/off
	invuln = 0;
	uni = 0;
	volt = 0;
	stick = 0;

	//for debug display
	changed = 0;

	move = 1;

	movementX = 0;
	movementY = 0;
}


void misc::invulnerable()
{
	BYTE* addr = (BYTE*)0x00422874;
	BYTE data[5] = {0x8B,0x0D,0x1C,0x38,0x45};
	BYTE* addr2 = (BYTE*)0x00422954;
	
	if(invuln)
	{
		MemPatch( addr, data, 5);
		//MemPatch(addr2, data, 5);
	}
	else
	{
		for(int i = 0 ; i < 5 ; i++) data[i] = 0x00;
		MemPatch(addr,data,5);
		//MemPatch(addr,data,5);
	}

	invuln = !invuln;
	changed = 1;
}

void misc::unicycle()
{
	//left wheel and right wheel
	BYTE* addr1 = (BYTE*)0x004537D6;
	BYTE* addr2 = (BYTE*)0x004537EE;
	BYTE data1[2] = {0xEB,0xBF};
	BYTE data2[2] = {0xEB,0x3F};
	
	if(uni)
	{
		MemPatch( addr1, data1, 2);
		MemPatch( addr2, data2, 2);
	}
	else
	{
		data1[0] = 0x00; data1[1] = 0x00;
		MemPatch( addr1, data1, 2);
		MemPatch( addr2, data1, 2);
	}
	uni = !uni;
	changed = 1;
}

void misc::voltDelay()
{
	BYTE* addr = (BYTE*)0x004537F8;
	BYTE data[8] = {0x9A, 0x99, 0x99, 0x99, 0x99, 0x99, 0xD9, 0x3F};
	
	if(volt)
	{
		MemPatch(addr, data, 8);
	}
	else
	{
		for(int i = 0 ; i < 8 ; i++) data[i] = 0x00;
		MemPatch(addr, data, 8);	
	}
	volt = !volt;
	changed = 1;

}


void misc::stickToGround()
{
	BYTE* addr = (BYTE*)0x00402BB0;
	BYTE data[8] = {0x75, 0x09};
				
	if(stick)
	{
		MemPatch(addr, data, 2);
	}
	else
	{
		data[0] = 0xEB; data[1] = 0x09;
		MemPatch(addr, data, 2);
	}
	stick = !stick;
	changed = 1;
}

void misc::display()
{

	if(changed)
	{
		system("cls");

		cout << "Invunerability: " ;
		if(invuln) 
			cout << "On" << endl;
		else cout << "Off" << endl;

		cout << "Unicycle: " ;
		if(uni) 
			cout << "On" << endl;
		else cout << "Off" << endl;

		cout << "No delay in volts: " ;
		if(volt) 
			cout << "On" << endl;
		else cout << "Off" << endl;

		cout << "Stuck to floor: " ;
		if(stick) 
			cout << "On" << endl;
		else cout << "Off" << endl;

		cout << "Elasticity: " << *(double*)0x00453758 << endl;

	}
	changed = 0;
}

void misc::elasticity(bool cond)
{
	BYTE Buff[10];
	for(int i = 0 ; i < 10 ; i++)
		Buff[i] = 0x90;

	BYTE* Dest = (BYTE*)0x00401B04;
	MemPatch(Dest,Buff,10);


	BYTE* addr = (BYTE*)0x00453758;
	
	double val = *(double*)0x00453758;

	//cond = 1, augment, else decrement
	if(cond)
		val += 100;
	else 
	{
		if(val > 0) //making it go under 0 will make it crash pretty fast and it's kinda useless
			val -= 100;
	}

	MemPatchTemplate(addr,&val,8);
	changed = 1;
}


void misc::changeWheelPos(double valL,double valR){

	wheelLeftChangePositionX(valL);
	wheelRightChangePositionX(valR);
}

//function which modifies the x/y coordinades of the bike/wheels/head.
//todo: move bike to object (apple)?
void misc::moveBike(double x,double y)
{
	//				bike		left wheel	right wheel head
	int addrX[] = { 0x00453A20, 0x00453A88, 0x00453Af0, 0x00453b30 };
	int addrY[] = { 0x00453A28, 0x00453A90, 0x00453Af8, 0x00453b38 };

	for (int i = 0; i < 4; i++)
	{
		*(double*)addrX[i] += x;
		*(double*)addrY[i] += y;
	}
}

void misc::squeeze()
{
	//				bike		left wheel	right wheel head
	int addrX[] = { 0x00453A20, 0x00453A88, 0x00453Af0, 0x00453b30 };
	int addrY[] = { 0x00453A28, 0x00453A90, 0x00453Af8, 0x00453b38 };

	//move all componants to the left wheel (start position)
	for (int i = 0; i < 4; i++)
	{
		if (!i)
		{ //body same place to wheel = crash
			*(double*)addrX[i] = *(double*)addrX[1] + 0.01;
			*(double*)addrY[i] = *(double*)addrY[1] + 0.01;
		}
		else if (i != 3)
		{
			*(double*)addrX[i] = *(double*)addrX[1] ;
			*(double*)addrY[i] = *(double*)addrY[1] ;
		}
		else //head offset is weird
		{
			*(double*)addrX[i] = *(double*)addrX[1];
			*(double*)addrY[i] = *(double*)addrY[1]-0.65;
		}
	}
	
}

//fuckkkkkkkkkkkkkkkk
//function which disables the physics applied on the wheels/head/bike
//There's probably a faster way, but too late.
void misc::disableMovement()
{
	BYTE buff[3];
	BYTE* dest3Byte[] = { (BYTE*)0x004028C4, (BYTE*)0x004028CA, (BYTE*)0x004028BE, (BYTE*)0x00402786, (BYTE*)0x004028BE, (BYTE*)0x00402A44, (BYTE*)0x0040278C, (BYTE*)0x00402792, 
		(BYTE*)0x004028C4, (BYTE*)0x004028CA, (BYTE*)0x00402A4A, (BYTE*)0x00402A50, (BYTE*)0x00403C48, (BYTE*)0x0040358B, (BYTE*)0x00403C4E, (BYTE*)0x00403C54, (BYTE*)0x00403591, 
		(BYTE*)0x00403597, (BYTE*)0x004027D1 };

	BYTE* dest2Byte[] = { (BYTE*)0x004028B9, (BYTE*)0x00402781, (BYTE*)0x004028B9, (BYTE*)0x00402A3F, (BYTE*)0x00403C43, (BYTE*)0x00403586};

	for (int i = 0; i < 3; i++)
		buff[i] = 0x90;

	for (int i = 0; i < 19; i++)
		MemPatch(dest3Byte[i], buff, 3);
	
	for (int i = 0; i < 6; i++)
		MemPatch(dest2Byte[i], buff, 2);


	
}


void misc::setMove(bool value)
{
	move = value;
}

bool misc::canMove()
{
	return move;
}

void misc::hitWall()
{
	if (*(BYTE*)0x00499108 == 1)
	{
		setMove(1);
	}
}
