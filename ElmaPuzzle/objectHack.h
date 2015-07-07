#ifndef obj_h
	#define obj_h
#include "Mem.h"
#include "windows.h"
#include <vector>
#include <iostream>
#include "misc.h"

using namespace std;

void __declspec() objInject();
void __declspec() collisionObjInject();
void __declspec() moveBikeInject();

class objects{

public:
	static objects obj;
	objects();

	struct pos{
		unsigned int x ;
		unsigned int y ;
	};

	bool reload ;
	

private:
	int curObject;
	char oldLev[8];
	double intensity;

public:
	void changeObject();
	void nextObject();
	void moveObject(int);
	pos getLocation();
	void clearVals();

	void newLocation(pos);

	bool newLev();

	
	void gravIntensity(int objID);
};

#endif