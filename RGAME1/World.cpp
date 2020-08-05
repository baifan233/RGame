#include "World.h"

void World::Init(GDevices* gdevices, ResourceManager* rm)
{
	if (rm == nullptr)
		return;
	resourceManager = rm;
	int width = 60000;
	int height = 4000;
	if (gdevices != nullptr)
	{
		this->gDevices = gdevices;
	}
	else
		return;
	cpVect gravity = cpv(0, 1000);
	space = cpSpaceNew();
	//cpSpaceSetCollisionPersistence(space,50);
	//cpfpow(0.1,1/60);
	cpSpaceSetGravity(space, gravity);
	cpSpaceSetSleepTimeThreshold(space, 2.0f);
	cpSpaceSetCollisionSlop(space, 0.1f);
	cpSpaceSetIterations(space, 30);
	RObject obj;  //µØ°å
	//obj.AddStaticShapeAsBox(cpv(0, 520), cpv(width, 520), space, 1.0, 80);
	obj.AddShapeAsBox(0, 0, width, 800, space, 1.0, GetUsableId(), RObjectMaterial::BASELAND);
	cpBodySetType(obj.GetBody(), CP_BODY_TYPE_STATIC);
	bodys.push_back(obj);
	cpSpaceSetCollisionBias(space, cpfpow(0.7f, 60.0f));


	RObject obj2;  //ºÐ×ÓÓÒ±ß
	obj2.AddShapeAsBox((width / 2 - (double)500), -height, 1000, height, space, 1.0, GetUsableId(), RObjectMaterial::BASELAND);
	bodys.push_back(obj2);

	RObject obj3;  //ºÐ×Ó×ó±ß
	obj3.AddShapeAsBox(-(width / 2 - 500), -height, 1000, height, space, 1.0, GetUsableId(), RObjectMaterial::BASELAND);
	bodys.push_back(obj3);

	RObject obj4;   //¶¥¸Ç
	obj4.AddShapeAsBox(0, -(height + 800), width + (double)2000, 800, space, 1.0, GetUsableId(), RObjectMaterial::BASELAND);
	bodys.push_back(obj4);

	InitPlayer();

	gDevices->g_GetD2DRen()->CreateSpriteBatch(&batch1);
}

void World::InitPlayer()
{
	player.AddShapeAsBox(0, 0, 10, 10, space, 0.7f, GetUsableId(), RObjectMaterial::WOOD);
	player.CreateGeometry(player.GetShape(), gDevices->g_GetFactory());
	bodys.push_back(player);

}

void World::Draw(RObjectType objType, float scaleSize)
{

	gDevices->g_GetD2DRen()->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
	batch1->Clear();


	D2D1_RECT_F rect1 = { 0.0f,0.0f,1024.0f,768.0f };

	batch1->AddSprites(1, &rect1);

	gDevices->g_GetD2DRen()->DrawSpriteBatch(
		batch1, resourceManager->GetTexture("bmp1").bitmap, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

	cpShapeType shapeType;
	ID2D1PathGeometry* geometry = NULL;
	ID2D1TransformedGeometry* transGeo = 0;
	cpPolyShape* poly;
	vector<D2D1_POINT_2F> pointsv;
	cpVect ptemp;
	int index = 0;
	float angle = 0.0f;
	cpVect pos = { 0.0 };
	D2D1_POINT_2F pointemp = { 0 };
	D2D1_RECT_F cameraRect = gDevices->GetCameraRect();
	ID2D1Layer* layer;
	D2D1::Matrix3x2F matrix;
	D2D1::Matrix3x2F matrixNormal;
	D2D1::Matrix3x2F BoxMatrix;
	matrixNormal = matrixNormal.Identity();
	ID2D1BitmapBrush* brush = resourceManager->GetTRS(0)->brush;
	gDevices->g_GetD2DRen()->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	for (size_t i = 0; i < bodys.size(); i++)
	{
		if (objType == RObjectType::All && RObjectType::DynamicsBox == bodys[i].GetType())
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
				angle = (float)cpBodyGetAngle(bodys[i].GetBody());
				matrix = matrix.Scale(D2D1::SizeF(scaleSize, scaleSize), { 0.0f,0.0f });
				gDevices->g_GetD2DRen()->SetTransform(matrix);
				pointemp = SpaceToScreen((float)pos.x, (float)pos.y, cameraRect);

				pos = { pointemp.x,pointemp.y };
				if (!bodys[i].HaveGeometry())
				{
					poly = (cpPolyShape*)bodys[i].GetShape();
					index = cpPolyShapeGetCount(bodys[i].GetShape());
					for (size_t j = 0; j < (size_t)index; j++)
					{
						ptemp = cpPolyShapeGetVert(bodys[i].GetShape(), j);
						float tempa = 0.9f;
						if (ptemp.x > 0)ptemp.x +=tempa; else if (ptemp.x < 0) ptemp.x -= tempa;
						if (ptemp.y > 0)ptemp.y += tempa; else if(ptemp.y<0) ptemp.y -= tempa;
						pointsv.push_back({ (float)ptemp.x,(float)ptemp.y });
					}

					//geometry = GetPathGeometry(gDevices->GetFactory(), pointsv, index, { 0.0,0.0 });
					/*pointsv.clear();

					BoxMatrix = BoxMatrix.Translation((FLOAT)pos.x, (FLOAT)pos.y);

					if (geometry)
					{
						gDevices->GetFactory()->CreateTransformedGeometry(geometry, BoxMatrix * BoxMatrix.Rotation(RadToDeg(angle),
							{ (float)pos.x,(float)pos.y }), &transGeo);
						bodys[i].SetGeometry();
						ReleaseCom(geometry);
					}*/
					bodys[i].SetGeometry(GetPathGeometry(gDevices->g_GetFactory(), pointsv, index, { 0.0,0.0 }));
					pointsv.~vector();

				}
				else
				{

					BoxMatrix = BoxMatrix.Translation((FLOAT)pos.x, (FLOAT)pos.y);					
					gDevices->g_GetFactory()->CreateTransformedGeometry(bodys[i].GetGeometry(), BoxMatrix * BoxMatrix.Rotation(RadToDeg(angle),
							{ (float)pos.x,(float)pos.y }), &transGeo);


				}
				if (cpBodyIsSleeping(bodys[i].GetBody()))
					brush->SetOpacity(0.5f);
				else
					brush->SetOpacity(1.0f);

				
				if (transGeo)
				{
					/*gDevices->g_GetD2DRen()->CreateLayer(&layer);
					gDevices->g_GetD2DRen()->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(),transGeo),layer);
					gDevices->g_GetD2DRen()->DrawBitmap(resourceManager->GetTexture("stone").bitmap);
					gDevices->g_GetD2DRen()->PopLayer();
					layer->Release();*/
					gDevices->g_GetD2DRen()->FillGeometry(transGeo, brush);
				}


				//gDevices->GetD2DRen()->PushAxisAlignedClip();
				ReleaseCom(transGeo);

				break;
			case CP_NUM_SHAPES:
				break;
			default:
				break;
			}

		}
	}
	gDevices->g_GetD2DRen()->SetTransform(matrixNormal);

}

