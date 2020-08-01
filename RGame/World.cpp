#include "World.h"

void World::Init(float width,float height)
{
	cpVect gravity = cpv(0, 1000);
	space = cpSpaceNew();
	//cpSpaceSetCollisionPersistence(space,50);
	//cpfpow(0.1,1/60);
	cpSpaceSetGravity(space, gravity);
	cpSpaceSetSleepTimeThreshold(space, 2);
	cpSpaceSetIterations(space, 15);
	RObject obj;
	obj.AddStaticShapeAsBox(cpv(0, height), cpv(width, height), space, 1.0, 80);
	//obj.AddShapeAsBox(0,0,width,-200,1000,space,1.0);
	//cpBodySetType(obj.GetBody(),CP_BODY_TYPE_STATIC);
	bodys.push_back(obj);	
}
