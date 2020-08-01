#pragma once
#include "RObject.h"
class Person :
	public RObject
{
private:
	int HP;  
	int MP;
	int VIT;  //耐力
	int hunger;  //饥饿度
	int force;//力量

public:
	void MoveX(float speed)
	{
		cpVect sp = cpBodyGetVelocity(body);
		sp.x = speed;
		cpBodySetVelocity(body, sp);
	}
	void MoveY(float speed)
	{
		cpVect sp = cpBodyGetVelocity(body);
		sp.y = speed;
		cpBodySetVelocity(body, sp);
	}
};

