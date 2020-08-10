#pragma once
#include"..\core\framework.h"
enum class RObjectType
{
	All = 0,
	StaticBox = 1,
	DynamicsBox = 2,
	Creature = 3,
	DynamicsCircle = 4,
	BaseLand = 5,
	Empty = 6
};
enum class RObjectMaterial
{
	WOOD = 0,
	STONE = 1,
	BASELAND = 2,
	Empty = 3,
};
static float RObjectMaterialGetMass(RObjectMaterial material)
{
	float mass = 0.0f;
	switch (material)
	{
	case RObjectMaterial::Empty:
		mass = 0;
		break;
	case RObjectMaterial::WOOD:
		mass = 10;
		break;
	case RObjectMaterial::STONE:
		mass = 25;
		break;
	case RObjectMaterial::BASELAND:
		mass = 5000;
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
	case RObjectMaterial::Empty:
		hardness = 0;
		break;
	case RObjectMaterial::WOOD:
		hardness = 1;
		break;
	case RObjectMaterial::STONE:
		hardness = 2;
		break;
	case RObjectMaterial::BASELAND:
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
	RObjectType type = RObjectType::Empty;
	float Width = 0.0f;
	float Height = 0.0f;
	float x = 0.0f;
	float y = 0.0f;
	float radius = 0.0f;
	int hardness = 0;
	float mass = 0.0f;
	cpBody* body = 0;
	RObjectMaterial material = RObjectMaterial::Empty;
	cpShape* shape = 0;
	ID2D1PathGeometry* geometry = 0;
public:
	void rePos(D2D1_RECT_F cameraRect)
	{
		cpShapeType shapeType;
		cpVect pos;
		D2D1_POINT_2F pointemp;
		cpPolyShape* poly;
		int index = 0;
		cpVect ptemp = { 0.0 };
		if (RObjectType::DynamicsBox == type)
		{

			shapeType = cpShapeGetShapeType(shape);
			switch (shapeType)
			{
			case cpShapeType::CP_CIRCLE_SHAPE:
				break;
			case cpShapeType::CP_SEGMENT_SHAPE:
				break;
			case cpShapeType::CP_POLY_SHAPE:
				pos = cpBodyGetPosition(body);
				//angle = (float)cpBodyGetAngle(bodys[i].GetBody());
				//matrix = matrix.Scale(D2D1::SizeF(scaleSize, scaleSize), { 0.0f,0.0f });
				//gDevices->g_GetD2DRen()->SetTransform(matrix);
				pointemp = SpaceToScreen((float)pos.x, (float)pos.y, cameraRect);

				pos = { pointemp.x,pointemp.y };

				poly = (cpPolyShape*)shape;
				index = cpPolyShapeGetCount(shape);
				for (size_t j = 0; j < (size_t)index; j++)
				{
					ptemp = cpPolyShapeGetVert(shape, (int)j);
					//float tempa = 0.9f;
					//if (ptemp.x > 0)ptemp.x += tempa; else if (ptemp.x < 0) ptemp.x -= tempa;
					//if (ptemp.y > 0)ptemp.y += tempa; else if (ptemp.y < 0) ptemp.y -= tempa;
					//pointsv.push_back({ (float)ptemp.x,(float)ptemp.y });
					//polygon->setVertex(j, vector2f((float)ptemp.x+pos.x, (float)ptemp.y+pos.y));
				}
			}
		}
	}

				//m_Polygon* GetPolyGon() { return polygon; }
				~RObject();
				RObjectType GetType() { return type; }
				void AddObj(cpBody * body, cpShape * shape)
				{
					this->body = body; this->shape = shape;
				}
				RObject() {};

				void SetId(int id) { cpBodySetUserData(body, (void*)&id); }
				void SetType(RObjectType otype) { type = otype; }
				void AddShapeAsBox(double x, double y, double width, double height, cpSpace * space, float friction, int, RObjectMaterial material);
				void AddShapeAsCircle(double x, double y, float radius, double mass, cpSpace * space, float friction);
				void AddStaticShapeAsBox(cpVect a, cpVect b, cpSpace * space, double friction, float radius);

				RObjectMaterial GetMaterial() { return material; }

				float GetMass() { return mass; }
				void SetMass(float mass) { this->mass = mass; }
				void SetMaterial(RObjectMaterial material)
				{
					this->material = material;
					hardness = RObjectMaterialGetHardness(material);
				};

				D2D1_RECT_F GetRectF();
				float GetRadius() { return radius; }
				cpBody* GetBody() { return body; };
				cpShape* GetShape() { return shape; };
				float GetWidth() { return Width; }
				float GetHeight() { return Height; }
				cpVect GetPosition()
				{
					if (type == RObjectType::DynamicsBox)
						return cpBodyGetPosition(body);
					else if (type == RObjectType::StaticBox)
						return { x,y };
					return { 0.0,0.0 };
				}

			};

