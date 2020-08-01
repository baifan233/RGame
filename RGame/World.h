#pragma once
#include"framework.h"
#include"RObject.h"
class World
{
private:
	vector<RObject>bodys;
	cpSpace* space;
public:
	void Init(float,float);
	void AddObject(RObject object) 
	{
		bodys.push_back(object);
	}
	void PhyStep(float dt) { cpSpaceStep(space, dt); }
	vector<RObject> GetBodys() { return bodys; }
	cpSpace* GetSpace() { return space; }
};

