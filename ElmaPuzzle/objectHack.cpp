#include "objectHack.h"

objects objects::obj;

objects::objects() {
	
   //object currently selected
   curObject = 0;

   //current lev check
   for(int i = 0 ; i < 8 ; i++)
	   oldLev[i] = 0;
   
   reload = 1;

}

 vector<int> objectAddr;
 vector<int> tempObjAddr;
 int tmpAddr;
 //vector which tells you if you already teleported to the apple, if so, you won't be able to teleport from it.
 vector<int> teleportedToApple;

void objects::changeObject()
{
	if(objectAddr.size())
	{
		//read current object type;
		BYTE* Dest = (BYTE*)(objectAddr[curObject]+32);
		BYTE type = *(BYTE*)Dest;
	
		//augment value and copy ( 0 < type < 5 ) or crash
		//1 = flower, 2 = apple, 3 = killer, 4 = start (only 1 start location or it crashes)
		type++;
		if(type > 4) type = 1;

		BYTE Buff[1] = {type};
		MemPatch(Dest,Buff,1);
	
	}
}


//function which takes the addresses of the dynamically stored objects 
int addr_00408DFA = 0x00408DFA;
void __declspec(naked) objInject()
{
	__asm{
		fld qword ptr [esi+8]
		fchs
		mov tmpAddr,esi
		mov [esi+0x10],eax
		fsub qword ptr [edi+0x20]
			
	}	
		
	tempObjAddr.push_back(tmpAddr);

	__asm{
		jmp addr_00408DFA
	}
}

void objects::nextObject()
{
	
	if(objectAddr.size())
	{
		//last object is always null, don't know what it is so I'm just removing it.
		if(curObject < objectAddr.size()-1)
			curObject++;
		else
			curObject = 0;

		//debug
		system("cls");
		cout << "object address: " << objectAddr[curObject] << endl;
		for (int i = 0; i <= 20; i++)
			cout << i*4 << " : " << *(int*)(objectAddr[curObject] + (i * 4)) << endl;

		cout << endl;
		cout << "Current Object: " << curObject;
		cout << "\tType: " ;

		
		switch(*(BYTE*)(objectAddr[curObject]+32))
		{
		case 1:
			cout << "Flower";
			break;
		case 2:
			cout << "Apple";
			break;
		case 3:
			cout << "Killer";
			break;
		case 4:
			cout << "Start";
			break;
		default:
			cout << "Unknown";
			break;
		}
		cout << endl;
	}
}

void objects::moveObject(int dir)
{
	if(objectAddr.size())
	{
		pos position = getLocation();
	
		//movement
		if		(dir == 0) position.y += objVelocity;
		else if (dir == 1) position.y -= objVelocity;
		else if (dir == 2) position.x -= objVelocity;
		else               position.x += objVelocity;
	
		newLocation(position);
	}
}


objects::pos objects::getLocation()
{
	int data;
	pos position ;

	int addr = objectAddr[curObject] + 16;
	
	//take the current position from the memory
	position.x = (*(int*)addr);
	position.y = (*(int*)(addr+4));
	return position;
}

void objects::newLocation(pos newPos)
{
	//copy object location x
	BYTE* Dest = (BYTE*)(objectAddr[curObject] + 16);
	MemPatchInt(Dest,&newPos.x,4);
	
	//copy object location y
	Dest += 4;
	MemPatchInt(Dest,&newPos.y,4);

}

void objects::clearVals()
{
	objectAddr = tempObjAddr;
	tempObjAddr.clear();
	curObject = -1;
	reload = 0;

	//teleportation
	teleportedToApple.clear();
	
	misc::mC.squeeze();

	misc::mC.setMove(1);

	*(double*)0x00453740 = 0; // no gravity
}


vector<int> collisionObj;
int objID;
int addr_0042E714 = 0x0042E713;

//collision with which object
void debugObjGrav(int tmp)
{
	if (tmp == -1) return;//collision with floor

	if (objectAddr.size())
	{
		if (*(int*)(objectAddr[tmp] + 32) == 1) return; //flower, cout will spam
		else if (*(int*)(objectAddr[tmp] + 32) == 3) cout << "Killer" << endl << endl;
		else //apple
		{
			cout << "Apple: ";
			cout << *(int*)(objectAddr[tmp] + 36) << endl;
			cout << "Gravity: ";
			switch (*(int*)(objectAddr[tmp] + 36))
			{
			case 0:
				cout << "NO GRAV" << endl;
				break;
			case 1:
				cout << "UP" << endl;
				break;
			case 2:
				cout << "DOWN" << endl;
				break;
			case 3:
				cout << "LEFT" << endl;
				break;
			case 4:
				cout << "RIGHT" << endl;
				break;
			default:
				cout << "Invalid gravity on object" << endl;
				break;
			}
			int anim = (*(int*)(objectAddr[tmp] + 40));

			cout << "Anim State: ";

			//anim can only be 0-8 (shown as 1-9 in game)
			if (anim >= 0 && anim < 9)
				cout << anim << endl;
			else
				cout << "Invalid object animation" << endl;
		}

		objects::obj.gravIntensity(tmp);
	}

}

//0x00453740 - double
//default value: 10
void objects::gravIntensity(int objID)
{
	if (*(int*)(objectAddr[objID] + 32) == 2) //apple
	{
		int anim = *(int*)(objectAddr[objID] + 40); //0-8
		
		switch (anim)
		{
		case 1:
			intensity = 0;
			break;
		case 2:
			intensity = 2;
			break;
		case 3:
			intensity = 5;
			break;
		case 4:
			intensity = 15;
			break;
		case 5:
			intensity = 25;
			break;
		case 6:
			intensity = 50;
			break;
		case 7:
			intensity = 100;
			break;
		case 8:
			intensity = 150;
			break;
		default:
			intensity = 10;
			break;

		}
			
		MemPatchTemplate((BYTE*)0x00453740, &intensity, 8);
	}
}



//function which teleports you from an apple to another, it will retain your momentum.
//When you touch an apple, it will teleport you to the next apple whos ID is just after the one you touched, if there isn't any, it does nothing.
void teleport(int objectID)
{
	if (objectID == -1) return;

	if (objectAddr.size() && *(int*)(objectAddr[objectID] + 32) == 2) //apple
	{
		//check if you already teleported to that apple, if so, "disable it".
		for (int i = 0; i < teleportedToApple.size(); i++)
		{
			//don't really need optimization to find.
			if (teleportedToApple[i] == objectID) return;
		}

		//start from the next object
		int nextObjID = objectID + 1;
		//find the next apple in the list
		for (; nextObjID < objectAddr.size(); nextObjID++)
		{
			if (*(int*)(objectAddr[nextObjID] + 32) == 2)
			{
				break;
			}
		}

		//no next apples found? do nothing
		if (nextObjID >= objectAddr.size())
			return;

		teleportedToApple.push_back(nextObjID);

		//object that you just touched
		int objLocX = *(int*)(objectAddr[objectID] + 16);
		int objLocY = *(int*)(objectAddr[objectID] + 20);

		//object teleported to
		int oldObjLocX = *(int*)(objectAddr[nextObjID] + 16);
		int oldObjLocY = *(int*)(objectAddr[nextObjID] + 20);
		
		//apples are ~29 units in diameter
		//0.806 is gotten by trial and error, the grid system of objects and player are weighted differently

		//calculate new x
		double newPosX = (double(oldObjLocX - objLocX)/ 29.0f *0.806);


		//calculate new Y
		double newPosY = (double(oldObjLocY - objLocY) / 29.0f *0.806);
	
		misc::mC.moveBike(newPosX, newPosY);
	}
}



//injected function which detects when you hit an object that's activated (flower/killer/apple/floor)
//start position are default deactivated, same with apples that you already touched
void __declspec(naked) collisionObjInject()
{
	__asm{ 
		push ebx
		mov ebx, 0x00487044
		mov [ebx], eax
		pop ebx
		//read which object id you touched.
		mov objID,edx
		
	}

	//to activate teleportation/object gravity, remove comments
	//debugObjGrav(objID);
	//teleport(objID);

	//go back to elma
	__asm{
		jmp addr_0042E714
	}
}

int addr_0x0041F870 = 0x0041F870;
int addr_0x00495A8E = 0x00495A8E;
void __declspec(naked) moveBikeInject()
{

	__asm{
		pushf
	}

	misc::mC.hitWall();

	if (!misc::mC.canMove())
	{
		misc::mC.moveBike(misc::mC.movementX, misc::mC.movementY);
	}
	else
	{
		if (*(BYTE*)0x00499108 == 1)
		{
			misc::mC.moveBike(-misc::mC.movementX, -misc::mC.movementY);
		}


		misc::mC.movementX = 0;
		misc::mC.movementY = 0;
	}

	__asm{
		popf

		call addr_0x0041F870
		
		jmp addr_0x00495A8E
	}
}
