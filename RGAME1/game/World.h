#pragma once
#include"..\core\framework.h"
#include"RObject.h"
#include"Player.h"
#include"..\core\GDevices.h"
#include"ResourceManager.h"

class World
{
private:
	vector<RObject>bodys;
	ResourceManager* resourceManager = nullptr;
	Player player;
	cpSpace* space = 0;
	GDevices* gDevices = 0;
	ID2D1SpriteBatch* batch1 = 0;
	bool bGravity = true;
	bool bbuildMode = false;
	//std::vector<m_Polygon*> polygons;
public:
	//void AddPolyGons(m_Polygon* p) { polygons.push_back(p); }
	//vector<m_Polygon*> GetPolyGons() { return polygons; }
	bool bBuildMode() { return bbuildMode; }
	void SetBuildMode(bool b) { bbuildMode = b; }
	bool IsGravityOn() { return bGravity; };
	GDevices* GetDevice() { return gDevices; };
	void Init(GDevices* gdevices, ResourceManager* rm);
	void InitPlayer();
	int GetUsableId()
	{
		int usable = 0;
		int* temp = 0;
		while (true)
		{
			if (bodys.size() > 0)
			{

				for (size_t i = 0; i < bodys.size(); i++)
				{
					if (bodys[i].GetType() == RObjectType::DynamicsBox)
					{
						if (temp = (int*)cpBodyGetUserData(bodys[i].GetBody()))
						{
							if (temp != nullptr)
							{
								usable = *temp;
								return usable;
							}
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
			if (bodys[i].GetType() == RObjectType::DynamicsBox)
			{
				if (id == *(int*)cpBodyGetUserData(bodys[i].GetBody()))
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
			if (bodys[i].GetType() == RObjectType::DynamicsBox)
			{
				if (id == *(int*)cpBodyGetUserData(bodys[i].GetBody()))
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
			if (bodys[i].GetType() == RObjectType::DynamicsBox)
			{
				if (id == *(int*)cpBodyGetUserData(bodys[i].GetBody()))
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
			if (bodys[i].GetType() == RObjectType::DynamicsBox)
			{
				if (id == *(int*)cpBodyGetUserData(bodys[i].GetBody()))
				{

					return bodys[i];
				}
			}
		}
		return RObject();
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
	}
	void SetGravity(float x, float y) { if (x == 0.0f && y == 0.0f)bGravity = false; else bGravity = true;   cpSpaceSetGravity(space, { x,y }); }

	void ClearSmall()
	{

		for (size_t i = 0; i < bodys.size(); i++)
		{
			if (bodys[i].GetType() == RObjectType::DynamicsBox)
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
	void Draw(RObjectType objType, float scaleSize);
};



#define MAX_VERTEXES_PER_VORONOI 16

struct WorleyContex {
	uint32_t seed;
	cpFloat cellSize;
	int width, height;
	cpBB bb;
	cpVect focus;
};
static inline cpVect
HashVect(uint32_t x, uint32_t y, uint32_t seed)
{
	//	cpFloat border = 0.21f;
	cpFloat border = 0.05f;
	uint32_t h = (x * (uint32_t)1640531513 ^ y * (uint32_t)2654435789) + seed;

	return cpv(
		cpflerp(border, 1.0f - border, (cpFloat)(h & 0xFFFF) / (cpFloat)0xFFFF),
		cpflerp(border, 1.0f - border, (cpFloat)((h >> 16) & 0xFFFF) / (cpFloat)0xFFFF)
	);
}

static cpVect
WorleyPoint(int i, int j, struct WorleyContex* context)
{
	cpFloat size = context->cellSize;
	int width = context->width;
	int height = context->height;
	cpBB bb = context->bb;

	//	cpVect fv = cpv(0.5, 0.5);
	cpVect fv = HashVect(i, j, context->seed);

	return cpv(
		cpflerp(bb.l, bb.r, 0.5f) + size * (i + fv.x - (double)width * 0.5f),
		cpflerp(bb.b, bb.t, 0.5f) + size * (j + fv.y - (double)height * 0.5f)
	);
}

static int
ClipCell(cpShape* shape, cpVect center, int i, int j, struct WorleyContex* context, cpVect* verts, cpVect* clipped, int count)
{
	cpVect other = WorleyPoint(i, j, context);
	//	printf("  other %dx%d: (% 5.2f, % 5.2f) ", i, j, other.x, other.y);
	if (cpShapePointQuery(shape, other, NULL) > 0.0f) {
		//		printf("excluded\n");
		memcpy(clipped, verts, count * sizeof(cpVect));
		return count;
	}
	else {
		//		printf("clipped\n");
	}

	cpVect n = cpvsub(other, center);
	cpFloat dist = cpvdot(n, cpvlerp(center, other, 0.5f));

	int clipped_count = 0;
	for (int j = 0, i = count - 1; j < count; i = j, j++) {
		cpVect a = verts[i];
		cpFloat a_dist = cpvdot(a, n) - dist;

		if (a_dist <= 0.0) {
			clipped[clipped_count] = a;
			clipped_count++;
		}

		cpVect b = verts[j];
		cpFloat b_dist = cpvdot(b, n) - dist;

		if (a_dist * b_dist < 0.0f) {
			cpFloat t = cpfabs(a_dist) / (cpfabs(a_dist) + cpfabs(b_dist));

			clipped[clipped_count] = cpvlerp(a, b, t);
			clipped_count++;
		}
	}

	return clipped_count;
}

static void
ShatterCell(cpSpace* space, cpShape* shape, cpVect cell, int cell_i, int cell_j, struct WorleyContex* context, RObjectMaterial material, World* world)
{
	//	printf("cell %dx%d: (% 5.2f, % 5.2f)\n", cell_i, cell_j, cell.x, cell.y);

	cpBody* body = cpShapeGetBody(shape);

	cpVect* ping = nullptr;
	ping = (cpVect*)_malloca(MAX_VERTEXES_PER_VORONOI * sizeof(cpVect));
	cpVect* pong = nullptr;
	pong = (cpVect*)_malloca(MAX_VERTEXES_PER_VORONOI * sizeof(cpVect));

	int count = cpPolyShapeGetCount(shape);
	count = (count > MAX_VERTEXES_PER_VORONOI ? MAX_VERTEXES_PER_VORONOI : count);
	if (ping == nullptr || pong == nullptr)
		return;
	for (int i = 0; i < count; i++) {
		ping[i] = cpBodyLocalToWorld(body, cpPolyShapeGetVert(shape, i));
	}

	for (int i = 0; i < context->width; i++) {
		for (int j = 0; j < context->height; j++) {
			if (
				!(i == cell_i && j == cell_j) &&
				cpShapePointQuery(shape, cell, NULL) < 0.0f
				) {
				count = ClipCell(shape, cell, i, j, context, ping, pong, count);
				memcpy(ping, pong, count * sizeof(cpVect));
			}
		}
	}

	cpVect centroid = cpCentroidForPoly(count, ping);
	cpFloat mass = cpAreaForPoly(count, ping, 0.0f) * RObjectMaterialGetMass(material);
	cpFloat moment = cpMomentForPoly(mass, count, ping, cpvneg(centroid), 0.0f);

	cpBody* new_body = cpSpaceAddBody(space, cpBodyNew(mass, moment));
	cpBodySetPosition(new_body, centroid);
	cpBodySetVelocity(new_body, cpBodyGetVelocityAtWorldPoint(body, centroid));
	cpBodySetAngularVelocity(new_body, cpBodyGetAngularVelocity(body));

	cpTransform transform = cpTransformTranslate(cpvneg(centroid));
	cpShape* new_shape = cpSpaceAddShape(space, cpPolyShapeNew(new_body, count, ping, transform, 0.0));
	// Copy whatever properties you have set on the original shape that are important
	cpShapeSetFriction(new_shape, cpShapeGetFriction(shape));
	RObject obj;
	obj.AddObj(new_body, new_shape);
	obj.SetMass((float)mass);
	obj.SetMaterial(material);
	obj.SetId(world->GetUsableId());
	obj.SetType(RObjectType::DynamicsBox);
	//obj.CreateGeometry(obj.GetShape(), world->GetDevice()->g_GetFactory());
	world->AddObj(obj);
}

static void
ShatterShape(cpSpace* space, cpShape* shape, cpFloat cellSize, cpVect focus, World* world)
{
	int id = *(int*)cpBodyGetUserData(cpShapeGetBody(shape));
	RObject obj = world->RGetObject(id);
	RObjectMaterial material = obj.GetMaterial();
	world->RemoveObj(id);
	world->Delete(id);

	cpBB bb = cpShapeGetBB(shape);
	int width = (int)((bb.r - bb.l) / cellSize) + 1;
	int height = (int)((bb.t - bb.b) / cellSize) + 1;
	//	printf("Splitting as %dx%d\n", width, height);
	struct WorleyContex context = { (uint32_t)rand(), cellSize, width, height, bb, focus };

	for (int i = 0; i < context.width; i++) {
		for (int j = 0; j < context.height; j++) {
			cpVect cell = WorleyPoint(i, j, &context);
			if (cpShapePointQuery(shape, cell, NULL) < 0.0f) {
				ShatterCell(space, shape, cell, i, j, &context, material, world);
			}
		}
	}
	//world.Free(id);


	//cpBodyFree(cpShapeGetBody(shape));
	//cpShapeFree(shape);
}
