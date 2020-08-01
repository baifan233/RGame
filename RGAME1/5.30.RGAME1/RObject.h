#pragma once
#include"framework.h"
enum RObjectType
{
	RObjectType_All = 0,
	RObjectType_StaticBox = 1,
	RObjectType_DynamicsBox = 2,
	RObjectType_Creature = 3,
	RObjectType_DynamicsCircle = 4,
	RObjectType_BaseLand = 5,
	RObjectType_Empty=6
};
enum RObjectMaterial
{
	RObjectMaterial_WOOD=0,
	RObjectMaterial_STONE=1,
	RObjectMaterial_BASELAND=2,
};
static float RObjectMaterialGetMass(RObjectMaterial material)
{
	float mass = 0.0f;
	switch (material)
	{
	case RObjectMaterial_WOOD:
		mass = 1;		
		break;
	case RObjectMaterial_STONE:
		mass = 3;
		break;
	case RObjectMaterial_BASELAND:
		mass = 1000;
		break;
	default:
		break;
	}
	return mass;
}
static int RObjectMaterialGetHardness(RObjectMaterial material)
{
	int hardness = 0;
	switch (material)
	{
	case RObjectMaterial_WOOD:
		hardness = 1;
		break;
	case RObjectMaterial_STONE:
		hardness = 2;
		break;
	case RObjectMaterial_BASELAND:
		hardness = 4000;
		break;
	default:
		break;
	}
	return hardness;
}
class RObject
{
protected:
	RObjectType type;
	float Width = 0.0f;
	float Height = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float radius = 0.0f;
	int hardness = 0;
	float mass = 0.0f;
	cpBody* body = 0;
	RObjectMaterial material;
	cpShape* shape = 0;
	ID2D1PathGeometry* geometry = 0;
public:
	~RObject();
	RObjectType GetType() { return type; }
	//void Init(RObjectType Type);
	void AddObj(cpBody* body, cpShape* shape)
	{
		this->body = body; this->shape = shape;
	}
	void CreateGeometry(cpShape* shape, ID2D1Factory4* factory)
	{
		cpPolyShape* poly = (cpPolyShape*)shape;
		int index = cpPolyShapeGetCount(shape);
		cpVect ptemp = { 0.0,0.0 };
		vector<D2D1_POINT_2F> pointsv;
		for (size_t j = 0; j < index; j++)
		{
			ptemp = cpPolyShapeGetVert(shape, j);
			pointsv.push_back({ (float)ptemp.x,(float)ptemp.y });
		}
		geometry = GetPathGeometry(factory, pointsv, index, { 0.0,0.0 });
		pointsv.clear();
	}
	ID2D1PathGeometry* GetGeometry()
	{
		if(geometry)return geometry;
	}
	bool HaveGeometry()
	{
		if (geometry == nullptr)return false; else return true;
	}

	void SetId(int id) { cpBodySetUserData(body, (void*)id); }
	void SetType(RObjectType otype) { type = otype; }
	void AddShapeAsBox(double x, double y, double width, double height,  cpSpace* space, float friction,int, RObjectMaterial material);
	void AddShapeAsCircle(double x, double y, float radius, double mass, cpSpace* space, float friction);
	void AddStaticShapeAsBox(cpVect a, cpVect b, cpSpace* space, double friction, float radius);
	
	RObjectMaterial GetMaterial() { return material; }

	float GetMass() { return mass; }
	void SetMass(float mass) { this->mass = mass; }
	void SetMaterial(RObjectMaterial material) 
	{
		this->material = material;
		hardness =RObjectMaterialGetHardness(material);
	};

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

