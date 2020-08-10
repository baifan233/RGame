#include "RObject.h"
RObject::~RObject()
{
	//if(geometry)this->geometry->Release();
	
}
void RObject::AddShapeAsBox(double x, double y, double width, double height,cpSpace* space,float friction,int id, RObjectMaterial material)
{
	type = RObjectType::DynamicsBox;
	this->x = (float)x;
	this->y= (float)y;
	this->Width = (float)width;
	this->Height = (float)height;
	this->material = material;
	mass =RObjectMaterialGetMass(material);
	hardness = RObjectMaterialGetHardness(material);;
	mass = mass*(float)width*(float)height;
	cpFloat moment = cpMomentForBox(mass,width,height);
	body = cpSpaceAddBody(space,cpBodyNew(mass,moment));
	shape = cpSpaceAddShape(space,cpBoxShapeNew(body,width,height,1));
	cpShapeSetFriction(shape, friction);
	cpBodySetUserData(body,(void*)&id);
	cpBodySetPosition(body, {x,y});		

}
void RObject::AddShapeAsCircle(double x, double y, float radius,double mass,cpSpace* space,float friction)
{
	type = RObjectType::DynamicsCircle;
	this->x = (float)x;
	this->y = (float)y;
	this->radius = radius;
		
	cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

	body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
	
	shape = cpSpaceAddShape(space, cpCircleShapeNew(body, radius, cpvzero));
	cpShapeSetFriction(shape,friction);
	cpBodySetPosition(body, { x,y });	

}
void RObject::AddStaticShapeAsBox(cpVect a,cpVect b, cpSpace* space, double friction,float radius)
{
	type = RObjectType::StaticBox;
	this->x = (float)a.x;
	this->y = (float)a.y;
	this->Width = (float)b.x- (float)a.x;
	this->Height = radius;
	shape =cpSegmentShapeNew(cpSpaceGetStaticBody(space),a,b,radius);
	
	cpShapeSetFriction(shape,friction);
	cpShapeSetElasticity(shape,0.0);
	cpSpaceAddShape(space,shape);
	body = cpShapeGetBody(shape);

}
D2D1_RECT_F RObject::GetRectF()
{
	D2D1_RECT_F rect = {0.0f};
	/*cpVect pos = cpBodyGetPosition(body);
	x=pos.x;
	y = pos.y;*/
	rect.left = x;
	rect.top = y;
	rect.right = x+Width;
	rect.bottom = y+Height;

	return rect;
}

