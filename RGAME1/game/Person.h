#pragma once
#include "RObject.h"
class Person :
	public RObject
{
private:
	int HP=0;
	int MP=0;
	int VIT=0;  //耐力
	int hunger=0;  //饥饿度
	int force=0;//力量
	bool grounded = false;
public:
	void Jump(float speed)
	{
		if (grounded)
		{
			cpVect sp = cpBodyGetVelocity(body);
			sp.y += speed;

			cpBodySetVelocity(body, sp);
		}
	}
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

