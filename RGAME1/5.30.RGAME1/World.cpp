#include "World.h"

void World::Init(float width, float height)
{
	cpVect gravity = cpv(0, 1000);
	space = cpSpaceNew();
	//cpSpaceSetCollisionPersistence(space,50);
	//cpfpow(0.1,1/60);
	cpSpaceSetGravity(space, gravity);
	cpSpaceSetSleepTimeThreshold(space, 0.5f);
	cpSpaceSetCollisionSlop(space, 0.5f);
	cpSpaceSetIterations(space, 30);
	RObject obj;  //µØ°å
	//obj.AddStaticShapeAsBox(cpv(0, 520), cpv(width, 520), space, 1.0, 80);
	obj.AddShapeAsBox(0, 0, width, 800, space, 1.0, GetUsableId(), RObjectMaterial_BASELAND);
	cpBodySetType(obj.GetBody(), CP_BODY_TYPE_STATIC);
	bodys.push_back(obj);
	cpSpaceSetCollisionBias(space, cpfpow(0.7f, 60.0f));
	

	RObject obj2;  //ºÐ×ÓÓÒ±ß
	obj2.AddShapeAsBox((width / 2 - 500), -height, 1000, height, space, 1.0, GetUsableId(), RObjectMaterial_BASELAND);
	bodys.push_back(obj2);

	RObject obj3;  //ºÐ×Ó×ó±ß
	obj3.AddShapeAsBox(-(width / 2 - 500), -height, 1000, height, space, 1.0, GetUsableId(), RObjectMaterial_BASELAND);
	bodys.push_back(obj3);

	RObject obj4;   //¶¥¸Ç
	obj4.AddShapeAsBox(0, -(height + 800), width + 2000, 800, space, 1.0, GetUsableId(), RObjectMaterial_BASELAND);
	bodys.push_back(obj4);

	InitPlayer();
}

void World::InitPlayer()
{
	player.AddShapeAsBox(0, 0, 90, 90, space, 0.7,GetUsableId(), RObjectMaterial_WOOD);
	bodys.push_back(player);
}

void World::Draw(ID2D1DeviceContext3* d2dContext, ID2D1Factory4* factory, RObjectType objType, ID2D1Brush* brush, D2D1_RECT_F cameraRect,ID2D1Bitmap* bitmap)
{
	cpShapeType shapeType;
	ID2D1PathGeometry* geometry=NULL;
	ID2D1TransformedGeometry* transGeo = 0;
	cpPolyShape* poly;
	vector<D2D1_POINT_2F> pointsv;
	cpVect ptemp;
	int index = 0;
	float angle = 0.0f;
	cpVect pos = { 0.0 };
	D2D1_POINT_2F pointemp = { 0 };
	D2D1::Matrix3x2F matrix;
	D2D1::Matrix3x2F matrixNormal;
	D2D1::Matrix3x2F BoxMatrix;
	matrixNormal = matrixNormal.Identity();
	for (size_t i = 0; i < bodys.size(); i++)
	{
		if (objType == 0 && RObjectType_DynamicsBox == bodys[i].GetType())
		{

			shapeType = cpShapeGetShapeType(bodys[i].GetShape());
			float pwidth = bodys[i].GetWidth();
			float pheight = bodys[i].GetHeight();
			switch (shapeType)
			{
			case CP_CIRCLE_SHAPE:
				break;
			case CP_SEGMENT_SHAPE:
				break;
			case CP_POLY_SHAPE:
				pos = cpBodyGetPosition(bodys[i].GetBody());
				if (btemp)
				{
					if (pos.x > cameraRect.right || pos.x < cameraRect.left)
						continue;
				}					
				angle = cpBodyGetAngle(bodys[i].GetBody());												
				matrix =matrix.Scale(D2D1::SizeF(scaleSize, scaleSize), { 0.0f,0.0f });
				d2dContext->SetTransform(matrix);
				pointemp = SpaceToScreen(pos.x, pos.y, cameraRect);
				pos = { pointemp.x,pointemp.y };
				if (!bodys[i].HaveGeometry())
				{
					poly = (cpPolyShape*)bodys[i].GetShape();
					index = cpPolyShapeGetCount(bodys[i].GetShape());

					for (size_t j = 0; j < index; j++)
					{
						ptemp = cpPolyShapeGetVert(bodys[i].GetShape(), j);
						pointsv.push_back({ (float)ptemp.x,(float)ptemp.y });
					}

					geometry = GetPathGeometry(factory, pointsv, index, { 0.0,0.0 });
					pointsv.clear();

					BoxMatrix = BoxMatrix.Translation((FLOAT)pos.x, (FLOAT)pos.y);

					if (geometry)factory->CreateTransformedGeometry(geometry, BoxMatrix * BoxMatrix.Rotation(RadToDeg(angle),
						{ (float)pos.x,(float)pos.y }), &transGeo);
				}
				else
				{
					
					BoxMatrix = BoxMatrix.Translation((FLOAT)pos.x, (FLOAT)pos.y);
					if (geometry)factory->CreateTransformedGeometry(bodys[i].GetGeometry(), BoxMatrix * BoxMatrix.Rotation(RadToDeg(angle),
						{ (float)pos.x,(float)pos.y }), &transGeo);
				}

				

				if (cpBodyIsSleeping(bodys[i].GetBody()))
					brush->SetOpacity(0.5f);
				else
					brush->SetOpacity(1.0f);
				d2dContext->FillGeometry(transGeo, brush);
				transGeo->Release();
				brush->SetOpacity(1.0f);
				
				
				
				break;
			case CP_NUM_SHAPES:
				break;
			default:
				break;
			}
		}
	}
	d2dContext->SetTransform(matrixNormal);
}

