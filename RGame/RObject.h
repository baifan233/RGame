#pragma once
#include"framework.h"
enum RObjectType
{
	RObjectType_StaticBox = 0,
	RObjectType_DynamicsBox = 1,
	RObjectType_Creature = 2,
	RObjectType_DynamicsCircle = 3,
	RObjectType_StaticCircle = 4,
	RObjectType_BaseLand = 5	
};
class RObject
{
protected:
	RObjectType type;
	float Width;
	float Height;
	float x;
	float y;
	float radius;
	cpBody* body;
	cpShape* shape;
public:
	~RObject();
	RObjectType GetType() { return type; }
	//void Init(RObjectType Type);
	void AddShapeAsBox(double x, double y, double width, double height, double mass, cpSpace* space, float friction);
	void AddShapeAsCircle(double x, double y, float radius, double mass, cpSpace* space, float friction);
	void AddStaticShapeAsBox(cpVect a, cpVect b, cpSpace* space, double friction, float radius);
	D2D1_RECT_F GetRectF();
	//cpVect GetPos() { return{ (double)x,(double)y }; }
	float GetRadius() { return radius; }
	cpBody* GetBody() { return body; };
	cpShape* GetShape() { return shape; };
	float GetWidth() { return Width; }
	float GetHeight() { return Height; }
	cpVect GetPosition()
	{
		if (type == RObjectType_DynamicsBox)
			return cpBodyGetPosition(body);
		else if (type == RObjectType_StaticBox)
			return { x,y };
	}	
};

