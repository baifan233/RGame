#pragma once
#include"framework.h"
#include"RObject.h"
#include"Player.h"
class World
{
private:
	vector<RObject>bodys;
	Player player;
	cpSpace* space;
	float scaleSize = 1.0f;
public:
	void SetScaleSize(float size) { scaleSize = size; }
	void Init(float, float);
	void InitPlayer();
	int GetUsableId()
	{
		int usable = 0;
		while (true)
		{
			if (bodys.size() > 0)
			{

				for (size_t i = 0; i < bodys.size(); i++)
				{
					if (bodys[i].GetType() == RObjectType_DynamicsBox)
					{
						if (usable != (int)cpBodyGetUserData(bodys[i].GetBody()))
						{
							return usable;
						}
					}
				}
				return usable;
			}
			else
			{
				return 0;
			}
			++usable;
		}
		return -1;
	}
	
	Player GetPlayer() { return player; }
	void RemoveObj(int id)
	{
		for (size_t i = 0; i < bodys.size(); i++)
		{
			if (bodys[i].GetType() == RObjectType_DynamicsBox)
			{
				if (id == (int)cpBodyGetUserData(bodys[i].GetBody()))
				{
					cpSpaceRemoveShape(space, bodys[i].GetShape());
					cpSpaceRemoveBody(space, cpShapeGetBody(bodys[i].GetShape()));
					return;
				}
			}
		}
	};
	void Delete(int id)
	{
		for (size_t i = 0; i < bodys.size(); i++)
		{
			if (bodys[i].GetType() == RObjectType_DynamicsBox)
			{
				if (id == (int)cpBodyGetUserData(bodys[i].GetBody()))
				{
					bodys.erase(bodys.begin() + i);
					return;
				}
			}
		}
	};
	void Free(int id)
	{
		for (size_t i = 0; i < bodys.size(); i++)
		{
			if (bodys[i].GetType() == RObjectType_DynamicsBox)
			{
				if (id == (int)cpBodyGetUserData(bodys[i].GetBody()))
				{
					cpBodyFree(bodys[i].GetBody());
					cpShapeFree(bodys[i].GetShape());
					return;
				}
			}
		}
	};
	bool btemp = false;
	void SetTemp(bool temp) { btemp = temp; };
	RObject RGetObject(int id)
	{
		for (size_t i = 0; i < bodys.size(); i++)
		{
			if (bodys[i].GetType() == RObjectType_DynamicsBox)
			{
				if (id == (int)cpBodyGetUserData(bodys[i].GetBody()))
				{

					return bodys[i];
				}
			}
		}
	};

	size_t GetBodySize() { return bodys.size(); }
	void AddObject(RObject object)
	{
		bodys.push_back(object);
	}
	void AddObj(RObject obj) { cpBodySetUserData(obj.GetBody(), (void*)bodys.size()); bodys.push_back(obj); }
	void PhyStep(float dt)
	{
		cpSpaceStep(space, dt);
		/*cpVect postemp;
		for (size_t i = 0; i < bodys.size(); i++)
		{
			if (bodys[i].GetType()==RObjectType_DynamicsBox)
			{
				postemp = cpBodyGetPosition(bodys[i].GetBody());
				if (postemp.y > 6000)
				{
					cpSpaceRemoveShape(space, bodys[i].GetShape());
					cpSpaceRemoveBody(space, cpShapeGetBody(bodys[i].GetShape()));
					bodys.erase(bodys.begin() + i);
				}
			}
		}*/
	}
	void SetGravity(float x, float y) { cpSpaceSetGravity(space, { x,y }); }

	void ClearSmall()
	{

		for (size_t i = 0; i < bodys.size(); i++)
		{
			if (bodys[i].GetType() == RObjectType_DynamicsBox)
			{
				cpBB bb = cpShapeGetBB(bodys[i].GetShape());
				cpFloat cell_size = cpfmax(bb.r - bb.l, bb.t - bb.b) / 5.0f;
				if (cell_size <= 20.0f)
				{
					cpSpaceRemoveShape(space, bodys[i].GetShape());
					cpSpaceRemoveBody(space, cpShapeGetBody(bodys[i].GetShape()));
					bodys.erase(bodys.begin() + i);
				}
			}
		}
	}

	vector<RObject> GetBodys() { return bodys; }
	cpSpace* GetSpace() { return space; }
	void Draw(ID2D1DeviceContext3* ren, ID2D1Factory4* factory, RObjectType objType, ID2D1Brush* brush, D2D1_RECT_F cameraRect,ID2D1Bitmap*);
};

