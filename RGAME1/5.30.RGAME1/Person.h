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
	void MoveX(float speed, float maxSpeed)
	{
		cpVect sp = cpBodyGetVelocity(body);
		sp.x += speed;
		if (fabsf((float)sp.x) > maxSpeed)
		{
			if (sp.x < 0)
				sp.x = -maxSpeed;
			else
				sp.x = maxSpeed;
		}
		cpBodySetVelocity(body, sp);
	}
	void MoveY(float speed, float maxSpeed)
	{
		cpVect sp = cpBodyGetVelocity(body);
		sp.y += speed;
		if (fabsf((float)sp.y) > maxSpeed)
		{
			if (sp.y < 0)
				sp.y = -maxSpeed;
			else
				sp.y = maxSpeed;
		}
		cpBodySetVelocity(body, sp);
	}
};

