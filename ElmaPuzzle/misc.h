#ifndef misc_h
#define misc_h

class misc{
private:
	bool invuln;
	bool uni;
	bool volt;
	bool stick;

	bool changed;
	
	//always move unless you hit a wall.
	bool move;

	

public:
	static misc mC;
	misc();
	void invulnerable();

	void unicycle();

	void voltDelay();

	void stickToGround();

	void display();

	void elasticity(bool);

	void keepSpeed();
	//void antiCrash();

	void changeWheelPos(double,double);

	void moveBike(double x, double y);
	void squeeze();
	void disableMovement();
	void setMove(bool value);
	bool canMove();
	void hitWall();

	double movementX;
	double movementY;
};

#endif