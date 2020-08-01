#pragma once
#include "RObject.h"
class Person :
	public RObject
{
private:
	int HP;  
	int MP;
	int VIT;  //����
	int hunger;  //������
	int force;//����

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

